#include "build/temp/_test_Message.c"
#include "mock_List.h"
#include "MessageDef.h"
#include "Message.h"
#include "unity.h"


















void setUp(void)

{



}



void tearDown(void)

{

}















void test_KMessageInit_valid(void)

{

 ListInit(&MessageQueueList, ID_GROUP_MESSAGE_QUEUE)

}
