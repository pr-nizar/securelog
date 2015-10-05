#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <curl/curl.h>
#include <zlib.h>

// #include "curl/curl.h"
// #include "zlib/zlib.h"


/*
* Define here email and password; you'd better use an app password.
* https://support.google.com/accounts/answer/185833
* https://security.google.com/settings/security/apppasswords
*/
#define TO "myEmail@gmail.com"
#define PASS "myGmailPassword"

void prepareFile();
void prepareCA();
void prepareMail();
void sendMail();

int main(void)
{
	return 0;
}

void prepareFile() {
	/*
	** We will compress the log file securelog.log with zlib giving us the 
	** securelog.xxx compressed file that we will be sending by mail.
	** Based on zpipe.c provided with zlib; check the zlib/examples/ folder.
	** Turned down errors check for now..
	*/
	#define CHUNK 16384
	int flush;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK], out[CHUNK];
	FILE *source, *dest;

	source = fopen("securelog.log","r");
	dest = fopen("securelog.xxx","w");
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	(void)deflateInit(&strm, 9);
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			(void)deflate(&strm, flush);
			have = CHUNK - strm.avail_out;
			fwrite(out, 1, have, dest);
		} while (strm.avail_out == 0);
	} while (flush != Z_FINISH);
	(void)deflateEnd(&strm);
	fclose(source);
	fclose(dest);
}

void prepareCA() {
	/*
	** We will download our bundle of X.509 certificates from curl's website.
	** Based on http://stackoverflow.com/questions/3471122#3471329
	** Turned down errors check for now..
	*/
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://curl.haxx.se/ca/cacert.pem");
	FILE* file = fopen("cacert.pem", "w");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(file);
}

void prepareMail() {
	/*
	** This functions prepares the mail payload (headers, body and base64 encoded
	** attachment). The payload is saved as securelog.pld file that the function
	** sendMail() will be using later.
	*/
	char mailHeader[] = "To: " TO "\r\nSubject: Securelog\r\n"
						"Content-Type: multipart/mixed; boundary=AWESOME\r\n\r\n"
						"--AWESOME\r\n"
						"Content-Type: text/plain; charset=UTF-8\r\n\r\n"
						"See attachment.\r\n\r\n"
						"--AWESOME\r\n"
						"Content-Type: application/octet-stream;\r\n"
						"Content-Disposition: attachment; filename=securelog.xxx\r\n"
						"Content-Transfer-Encoding: base64\r\n\r\n";
	char mailFooter[] = "--AWESOME--";
	BIO *bio = NULL, *b64 = NULL;
	char *buffer = NULL;
	size_t bytesRead = NULL;
	FILE *file_in = NULL, *file_out = NULL;

	file_in = fopen("securelog.xxx", "rb");
	file_out = fopen("securelog.pld", "w");
	fwrite(mailHeader,sizeof(mailHeader)-1,1,file_out);
	fclose(file_out);
	file_out = 0;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new_file("securelog.pld","a");
	BIO_push(b64, bio);
	buffer = malloc(sizeof(char)*512);
	while ((bytesRead = fread(buffer, 1, 512, file_in)) > 0) BIO_write(b64, buffer, bytesRead);
	free(buffer);
	BIO_flush(b64);
	BIO_free_all(b64);

	file_out = fopen("securelog.pld", "a");
	fwrite(mailFooter,sizeof(mailFooter)-1,1,file_out);
	fclose(file_out);
}

void sendMail() {
	/*
	* Initialisation
	*/
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	curl = curl_easy_init();
	if(curl) {
		/*
		* Authentication with TO and PASS defined above.
		*/
		curl_easy_setopt(curl, CURLOPT_USERNAME, TO);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, PASS);
		/*
		* Gmail smtp
		* https://support.google.com/a/answer/176600
		*/
		curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
		/*
		* CURLUSESSL_ALL: Require SSL for all communication or fail with CURLE_USE_SSL_FAILED
		*/
		curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
		/*
		* CA bundle from http://curl.haxx.se/ca/cacert.pem ; we'll be updating it because we don't
		* know if some certificates has expired in the meantime or not.
		*/
		curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
		recipients = curl_slist_append(recipients, TO);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
		/*
		* We're using a callback function to specify the payload (the headers and
		* body of the message). You could just use the CURLOPT_READDATA option to
		* specify a FILE pointer to read from.
		*/
		FILE *mailTxt = fopen("securelog.pld", "r");
		curl_easy_setopt(curl, CURLOPT_READDATA, mailTxt);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		/*
		* Should we turn on debug mode?
		*/
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		/*
		* Send the message
		*/
		res = curl_easy_perform(curl);
		/*
		* Check for errors
		*/
		if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		/*
		* Free the list of recipients
		*/
		curl_slist_free_all(recipients);
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
}