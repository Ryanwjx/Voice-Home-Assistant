SOURCES += \
    $$PWD/Voice-LLM-Assistant.cpp

HEADERS += \
    $$PWD/Voice-LLM-Assistant.h

RESOURCES += \
    $$PWD/res_VoiceLLMAssistant.qrc

include($$PWD/../audiorecorder/audiorecorder.pri)
include($$PWD/../webapi-stt/stt.pri)
include($$PWD/../webapi-tts/tts.pri)
include($$PWD/../webapi-llm/llm.pri)