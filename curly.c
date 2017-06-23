#include "curly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// This is not curly anymore. Basically it is only a wrapper to fit exactly our needs
// Mainly the code is taken from a libcurl example: https://curl.haxx.se/libcurl/c/postinmemory.html

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

char *curl_do_post(const char *url, const char *postdata, const char *resolve_host, const long timeout, const char **headers)
{
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    
    chunk.memory = malloc(1);  /* will be grown as needed by realloc above */
    chunk.size = 0;    /* no data at this point */
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    
    char *result = NULL;
    
    if(curl) {
        
        struct curl_slist *headers_list = NULL;
        
        if(headers != NULL){
            while(*headers){
                headers_list = curl_slist_append(headers_list, *headers);
                ++headers;
            }
        }
        
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers_list);
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

        // bobybacs: added following line, maybe this will solve the crash in curl
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        
        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        
        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        
        /* some servers don't like requests that are made without a user-agent
         field, so we provide one */
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
        
        /* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
         itself */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postdata));
        
        if(resolve_host != NULL){
            struct curl_slist *host = NULL;
            host = curl_slist_append(NULL, resolve_host);
            curl_easy_setopt(curl, CURLOPT_RESOLVE, host);
        }
        
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res == CURLE_OK) {
            /*
             * Now, our chunk.memory points to a memory block that is chunk.size
             * bytes big and contains the remote file.
             *
             * Do something nice with it!
             */
             asprintf(&result,"%s",chunk.memory);
        }
        
        /* always cleanup */ 
        curl_easy_cleanup(curl);
        
        free(chunk.memory);
        
        /* we're done with libcurl, so clean it up */ 
        curl_global_cleanup();
    }
    return result;
}
