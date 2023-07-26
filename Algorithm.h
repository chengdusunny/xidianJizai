#pragma once

#include <grpcpp/grpcpp.h>

#include "include/interface.grpc.pb.h"
#include "include/interface.pb.h"

class Algorithm
{
public:
	Algorithm(std::shared_ptr<grpc::Channel> channel) : _stub(TaskPathPlan::NewStub(channel)) {}

	TaskPathPlanReply task_assign_path_plan(TaskPathPlanRequest& request);

private:
	std::unique_ptr<TaskPathPlan::Stub> _stub;
};

