message("--------------------->>>>> LLMConnector cmake start <<<<<---------------------")

find_package(AWSSDK REQUIRED COMPONENTS core bedrock-runtime)

target_link_libraries(modules PUBLIC  ${AWSSDK_LINK_LIBRARIES})
target_link_libraries(modules PUBLIC  ${AWSSDK_LINK_LIBRARIES})

# Include the AWS SDK headers
target_include_directories(modules PUBLIC  PRIVATE ${AWSSDK_INCLUDE_DIRS})
target_include_directories(modules PUBLIC  PRIVATE ${AWSSDK_INCLUDE_DIRS})

message("--------------------->>>>> LLMConnector cmake end <<<<<---------------------")