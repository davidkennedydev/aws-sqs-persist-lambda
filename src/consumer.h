#pragma once

#include <aws/core/Aws.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/memory/stl/AWSString.h>

#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>

#include "api.h"
#include "dynamodb.h"
#include "sqs.h"

std::string GetEnvironmentVariable(const char* name) {
  const char* value = std::getenv(name);
  if (!value) {
    std::stringstream error;
    error << "Expected environment variable " << name << " set.";
    throw std::runtime_error(error.str());
  }
  return value;
}

void ConsumeMessage() {
  Aws::SDKOptions options;
  SimpleAws::Api api(options);
  {
    using namespace SimpleAWS;
    Aws::Client::ClientConfiguration config;

    const auto QUEUE_URL = GetEnvironmentVariable("QUEUE_URL");
    Sqs sqs(config, QUEUE_URL);
    auto message = sqs.ReceiveMessage();

    DynamoDB db(config);
    const auto TABLE_NAME = GetEnvironmentVariable("TABLE_NAME");
    db.insert(TABLE_NAME,
              {{"Id", message.GetMessageId()}, {"Value", message.GetBody()}});

    sqs.DeleteMessage(message);
  }
}
