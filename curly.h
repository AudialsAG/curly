#ifndef CURLY_H_
#define CURLY_H_

#ifdef __cplusplus
extern "C" {
#endif

char *curl_do_post(const char *url, const char *postdata, const char *resolve_host, const char **headers);

#ifdef __cplusplus
}
#endif
#endif //CURLY_H_

