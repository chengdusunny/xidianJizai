// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: interface.proto

#include "interface.pb.h"
#include "interface.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>

static const char* TaskPathPlan_method_names[] = {
  "/TaskPathPlan/taskPathPlan",
};

std::unique_ptr< TaskPathPlan::Stub> TaskPathPlan::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< TaskPathPlan::Stub> stub(new TaskPathPlan::Stub(channel, options));
  return stub;
}

TaskPathPlan::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_taskPathPlan_(TaskPathPlan_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status TaskPathPlan::Stub::taskPathPlan(::grpc::ClientContext* context, const ::TaskPathPlanRequest& request, ::TaskPathPlanReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::TaskPathPlanRequest, ::TaskPathPlanReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_taskPathPlan_, context, request, response);
}

void TaskPathPlan::Stub::async::taskPathPlan(::grpc::ClientContext* context, const ::TaskPathPlanRequest* request, ::TaskPathPlanReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::TaskPathPlanRequest, ::TaskPathPlanReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_taskPathPlan_, context, request, response, std::move(f));
}

void TaskPathPlan::Stub::async::taskPathPlan(::grpc::ClientContext* context, const ::TaskPathPlanRequest* request, ::TaskPathPlanReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_taskPathPlan_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::TaskPathPlanReply>* TaskPathPlan::Stub::PrepareAsynctaskPathPlanRaw(::grpc::ClientContext* context, const ::TaskPathPlanRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::TaskPathPlanReply, ::TaskPathPlanRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_taskPathPlan_, context, request);
}

::grpc::ClientAsyncResponseReader< ::TaskPathPlanReply>* TaskPathPlan::Stub::AsynctaskPathPlanRaw(::grpc::ClientContext* context, const ::TaskPathPlanRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsynctaskPathPlanRaw(context, request, cq);
  result->StartCall();
  return result;
}

TaskPathPlan::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      TaskPathPlan_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< TaskPathPlan::Service, ::TaskPathPlanRequest, ::TaskPathPlanReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](TaskPathPlan::Service* service,
             ::grpc::ServerContext* ctx,
             const ::TaskPathPlanRequest* req,
             ::TaskPathPlanReply* resp) {
               return service->taskPathPlan(ctx, req, resp);
             }, this)));
}

TaskPathPlan::Service::~Service() {
}

::grpc::Status TaskPathPlan::Service::taskPathPlan(::grpc::ServerContext* context, const ::TaskPathPlanRequest* request, ::TaskPathPlanReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


