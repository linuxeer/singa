#include "singa.h"
#include "cJSON.h"

void getPets(FILE * fp_resp) {
    fprintf(fp_resp , "HTTP/1.1 200 ok\r\n");
    fprintf(fp_resp , "text/javascript\r\n");
    fprintf(fp_resp , "\r\n");
    // 构建json字符串
    cJSON * root , * fmt , * p_array;
    root = cJSON_CreateObject();
    p_array = cJSON_CreateArray();
    cJSON_AddItemToObject(root , "name" , cJSON_CreateString("Jack (\"Bee\") Nimble"));
    cJSON_AddItemToObject(root , "format" , fmt = cJSON_CreateObject());
    cJSON_AddStringToObject(fmt , "type" , "rect");
    cJSON_AddNumberToObject(fmt , "width" , 1920);
    cJSON_AddNumberToObject(fmt , "height" , 1080);
    cJSON_AddFalseToObject (fmt , "interlace");
    cJSON_AddNumberToObject(fmt , "frame rate" , 24);
    cJSON_AddItemToObject(root , "peoples" , p_array);
    char * rendered = cJSON_Print(root);
    fprintf(fp_resp , "%s" , rendered);
    cJSON_Delete(root);
    fclose(fp_resp);
};
void removePets(FILE * fp_resp) {
    fprintf(fp_resp , "HTTP/1.1 200 ok\r\n");
    fprintf(fp_resp , "text/javascript\r\n");
    fprintf(fp_resp , "\r\n");
    fprintf(fp_resp , "{\"removePetssuccess:\":true}");
    fclose(fp_resp);
};
struct ROUTE routes[10] = {
        {
                "/pets/html/getPets.do" ,
                getPets
        } ,
        {
                "/pet/html/removePets.do" ,
                removePets
        }
};
int main() {
    singa_init();
}
