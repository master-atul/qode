// Copyright 2017 Atul R. All rights reserved.

#include <string.h>

#include "qode.h"
#include "node/src/qode_shared.h"
#include "node/src/env-inl.h"
#include "src/integration/node_integration.h"

std::string qodeVersion = "2.0.0";

namespace qode {

// The global instance of NodeIntegration.
std::unique_ptr<NodeIntegration> g_node_integration;

inline v8::Local<v8::String> ToV8String(node::Environment *env,
                                        const std::string str) {
  return v8::String::NewFromUtf8(env->isolate(), str.c_str(),
                                 v8::NewStringType::kNormal,
                                 static_cast<int>(str.length()))
      .ToLocalChecked();
}

// Force running uv loop.
void ActivateUvLoop(const v8::FunctionCallbackInfo<v8::Value> &args) {
  g_node_integration->CallNextTick();
}

bool InitWrapper(node::Environment *env) {
  v8::HandleScope handle_scope(env->isolate());
  v8::Local<v8::Value> versions =
      env->process_object()
          ->Get(env->context(), ToV8String(env, "versions"))
          .ToLocalChecked();
  versions.As<v8::Object>()
      ->Set(env->context(), ToV8String(env, "qode"),
            ToV8String(env, qodeVersion))
      .ToChecked();

  env->SetMethod(env->process_object(), "activateUvLoop", &ActivateUvLoop);
  return true;
}

bool RunUvLoopOnceWrapper() {
  g_node_integration->UvRunOnce();
  return false;
}

int Start(int argc, char *argv[]) {
  qode::qode_argc = argc;
  qode::qode_argv = argv;
  // Prepare node integration.
  g_node_integration.reset(NodeIntegration::Create());
  g_node_integration->Init();
  // Set run loop and init function on node.
  qode::InjectQodeInit(&InitWrapper);
  qode::InjectQodeRunUvLoopOnce(&RunUvLoopOnceWrapper);
  // qode::InjectQodeRunLoop(&RunUvLoopOnceWrapper);

  // Always enable GC this app is almost always running on desktop.
  std::string qodeFlags = "--expose_gc";
  v8::V8::SetFlagsFromString(qodeFlags.c_str(), qodeFlags.length());

  // Start node js
  int code = node::Start(qode::qode_argc, qode::qode_argv);
  // Clean up node integration and quit.
  g_node_integration.reset();
  return code;
}

}  // namespace qode
