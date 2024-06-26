#ifndef LLMINTERFACE_H
#define LLMINTERFACE_H

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/bedrock-runtime/BedrockRuntimeClient.h>
#include <aws/bedrock-runtime/model/InvokeModelRequest.h>
#include <aws/bedrock-runtime/model/InvokeModelResult.h>
#include <aws/bedrock-runtime/model/ConverseRequest.h>
#include <aws/bedrock-runtime/model/ConverseResult.h>
#include <aws/bedrock-runtime/model/Message.h>
#include <aws/bedrock-runtime/model/ConversationRole.h>
#include <iostream>
#include <string>
#include <memory>

#include "Player.h"
#include "Creature.h"
#include "Config.h"

namespace DDLLM{

    // Aws::SDKOptions options;
    // Aws::InitAPI(options);

    // class LLMInitializer {
    // public:
    //     static void Initialize() {
    //         Aws::SDKOptions options;
    //         Aws::InitAPI(options);
    //         // ... any other initialization code ...
    //     }
    // };

    class LLMInterface {
    public:
        LLMInterface();
        ~LLMInterface();

        std::string invoke(const std::string& input_message);
        void invokeAsync(const std::string& input_message, Player* sender, Creature* receiver, Language lang);
        std::string player_say(const std::string& player_message);
        void player_say_async(const std::string& player_message, Player* sender, Creature* receiver, Language lang);

    private:
        Aws::Client::ClientConfiguration clientConfig;
        std::unique_ptr<Aws::BedrockRuntime::BedrockRuntimeClient> bedrockRuntimeClient;
        Aws::SDKOptions options;
    };

    class CustomAsyncCallerContext : public Aws::Client::AsyncCallerContext {
    public:
        CustomAsyncCallerContext(Player* sender, Creature* receiver, Language lang);

        Player* GetSender() const;
        Creature* GetReceiver() const;
        Language GetLanguage() const;

    private:
        Player* m_sender;
        Creature* m_receiver;
        Language m_lang;
    };

    void HandlerFunction(const Aws::BedrockRuntime::BedrockRuntimeClient* client,
                        const Aws::BedrockRuntime::Model::ConverseRequest& request,
                        const Aws::BedrockRuntime::Model::ConverseOutcome& outcome,
                        const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context);

} // namespace DDLLM

#endif // LLMINTERFACE_H
