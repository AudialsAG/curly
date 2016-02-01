/*
 * curly.h
 * Wrapper code for libcurl.
 *
 *  Created on: 31/03/2015
 *      Author: johan
 */

#ifndef CURLY_H_
#define CURLY_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* curly_http_transaction_handle;
typedef	void(*on_http_request_completed)(curly_http_transaction_handle handle, int http_response_code, void* data, int size);

void curly_dispose();

curly_http_transaction_handle curly_http_get(char* url, void* cb);
curly_http_transaction_handle curly_http_put(char* url, void* data, int size, void* cb);

#ifdef __cplusplus
}
#endif
#endif //CURLY_H_

