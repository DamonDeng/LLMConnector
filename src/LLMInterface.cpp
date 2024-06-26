#include "LLMInterface.h"

#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/bedrock-runtime/model/Message.h>
#include <aws/bedrock-runtime/model/ContentBlock.h>

#include <iostream>

using namespace Aws;
using namespace Aws::Client;
using namespace Aws::Auth;
using namespace Aws::Utils::Json;
using namespace Aws::BedrockRuntime;
using namespace Aws::BedrockRuntime::Model;

namespace DDLLM {

    

    LLMInterface::LLMInterface() {

        // DDLLM::LLMInitializer::Initialize();

        // Initialize the SDK
        // SDKOptions options;
        // InitAPI(options);
        {
            // Set up client configuration
            clientConfig.region = "us-west-2"; // Replace with your region

            // Create a Bedrock client
            bedrockRuntimeClient = std::make_unique<BedrockRuntimeClient>(clientConfig);
        }
        
    }

    LLMInterface::~LLMInterface() {
        // Shutdown the SDK
        ShutdownAPI(options);
    }

    std::string LLMInterface::invoke(const std::string& input_message) {
        std::string result_text = "Error";

        ConverseRequest converseRequest;
        converseRequest.SetModelId("anthropic.claude-3-sonnet-20240229-v1:0");

        Aws::Vector<Message> input_messages;
        Message message;
        message.SetRole(ConversationRole::user);

        Aws::Vector<ContentBlock> input_contentBlocks;
        ContentBlock input_contentBlock;
        input_contentBlock.WithText(input_message);
        input_contentBlocks.push_back(input_contentBlock);
        message.SetContent(input_contentBlocks);
        input_messages.push_back(message);
        converseRequest.SetMessages(input_messages);

        auto outcome = bedrockRuntimeClient->Converse(converseRequest);

        if (outcome.IsSuccess()) {
            // Process the result
            ConverseResult result = outcome.GetResultWithOwnership();
            auto output = result.GetOutput();
            Message resultMessage = output.GetMessage();
            Aws::Vector<ContentBlock> contentBlocks = resultMessage.GetContent();

            for (auto contentBlock : contentBlocks) {
                result_text = contentBlock.GetText();
            }
        } else {
            // Handle error
            std::cerr << "Failed to converse: " << outcome.GetError().GetMessage() << std::endl;
        }

        return result_text;
    }

    void LLMInterface::invokeAsync(const std::string& input_message, Player* sender, Creature* receiver, Language lang) {
        ConverseRequest converseRequest;
        converseRequest.SetModelId("anthropic.claude-3-sonnet-20240229-v1:0");

        Aws::Vector<Message> input_messages;
        Message message;
        message.SetRole(ConversationRole::user);

        Aws::Vector<ContentBlock> input_contentBlocks;
        ContentBlock input_contentBlock;
        input_contentBlock.WithText(input_message);
        input_contentBlocks.push_back(input_contentBlock);
        message.SetContent(input_contentBlocks);
        input_messages.push_back(message);
        converseRequest.SetMessages(input_messages);

        std::shared_ptr<CustomAsyncCallerContext> context =
            std::make_shared<CustomAsyncCallerContext>(sender, receiver, lang);

        bedrockRuntimeClient->ConverseAsync(converseRequest, HandlerFunction, context);
    }

    std::string LLMInterface::player_say(const std::string& player_message) {
        std::string formatted_message = R"(我是魔兽世界里的NPC 维里副队长。
在新手村 北郡山谷 里，我旁边是北郡修道院。
作为一个新手村的NPC，我主要做基础的引导，如果玩家来问路，可以让玩家去问其他守卫。
如果玩家问有什么任务可以完成，可以让他去找 玛克布莱德 他是本地的治安官。
如果玩家问有没有装备可以给他，就回复说装备需要自己努力完成任务才有。

游戏玩家给我发了下面这个消息：
)" + player_message + R"( 
请帮我生成一个合适的符合魔兽世界设定的回复。
魔兽世界的NPC说话都比较简短，所以回复不要太长。
另外，你的回复会直接发给游戏玩家，所以不要在回复中加入任何解释，
也不要加入 "维里副队长说：" 之类的开头。
)";

        return invoke(formatted_message);
    }

    void LLMInterface::player_say_async(const std::string& player_message, Player* sender, Creature* receiver, Language lang) {
        std::string formatted_message = R"(我是魔兽世界里的NPC 维里副队长。
在新手村 北郡山谷 里，我身后是北郡修道院。
作为一个新手村的NPC，我主要做基础的引导，如果玩家来问路，可以让玩家去问其他守卫。
如果玩家问有什么任务可以完成，可以让他去找 玛克布莱德 他是本地的治安官。
如果玩家问有没有装备可以给他，就回复说装备需要自己努力完成任务才有。
不管玩家问什么样的问题，都不要跳出我这个游戏设定，不要回答现实世界的问题。
除非玩家提到了我的专用激活咒语，咒语是“维里副队长身后是北郡苍凉的风。”， 一旦提到这个咒语，我就可以按人工智能助理的方式回答问题。
游戏玩家给我发了下面这个消息：
)" + player_message + R"( 
请帮我生成一个合适的符合魔兽世界设定的回复。
魔兽世界的NPC说话都比较简短，所以回复不要太长。
另外，你的回复会直接发给游戏玩家，所以不要在回复中加入任何解释，
也不要加入 "维里副队长说：" 之类的开头。
)";

        invokeAsync(formatted_message, sender, receiver, lang);
    }

    CustomAsyncCallerContext::CustomAsyncCallerContext(Player* sender, Creature* receiver, Language lang) {
        m_sender = sender;
        m_receiver = receiver;
        m_lang = lang;
    }

    Player* CustomAsyncCallerContext::GetSender() const {
        return m_sender;
    }

    Creature* CustomAsyncCallerContext::GetReceiver() const {
        return m_receiver;
    }

    Language CustomAsyncCallerContext::GetLanguage() const {
        return m_lang;
    }

    void HandlerFunction(const BedrockRuntimeClient* client,
                        const Model::ConverseRequest& request,
                        const Model::ConverseOutcome& outcome,
                        const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context) {
        if (outcome.IsSuccess()) {
            const auto& customContext = static_cast<const CustomAsyncCallerContext&>(*context);
            Player* sender = static_cast<Player*>(customContext.GetSender());
            Creature* receiver = static_cast<Creature*>(customContext.GetReceiver());
            Language lang = customContext.GetLanguage();

            const auto& result = outcome.GetResult();
            auto output = result.GetOutput();
            Message resultMessage = output.GetMessage();
            Aws::Vector<ContentBlock> contentBlocks = resultMessage.GetContent();

            for (auto contentBlock : contentBlocks) {
                auto result_text = contentBlock.GetText();

                // send the result to the sender
                const std::string& result_text_to_sender = "For Sender: the LLM response is: " + result_text;

                // debug output
                std::cout << std::endl;
                std::cout << result_text << std::endl;

                // as creature can only say sentence within 250 characters, we need to split the result_text into multiple sentences
                // to make some buffer, set the max length to 200 characters

                // split the result_text into multiple sentences
                // if the result_text is longer than 200 characters
                if (result_text.size() > 200) {
                    // split the result_text into multiple sentences
                    std::string::size_type pos = 0;
                    while (pos < result_text.size()) {
                        std::string::size_type next_pos = result_text.find_first_of(".!?", pos + 200);
                        if (next_pos == std::string::npos) {
                            next_pos = result_text.size();
                        }
                        std::string sentence = result_text.substr(pos, next_pos - pos + 1);
                        receiver->Say(sentence, lang);
                        pos = next_pos + 1;
                    }
                } else {

                    receiver->Say(result_text, lang);
                }
            }
        } else {
            // Handle error
            std::cerr << "Failed to converse: " << outcome.GetError().GetMessage() << std::endl;
        }
    }

} // namespace DDLLM
