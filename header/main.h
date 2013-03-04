#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h> //isspace

#define sgn(x) (x<0)?(-1):1
#define abs(x) (((x)<0)?(-(x)):(x))
#define min(a,b)  (a<b)?(a):(b)
#define max(a,b) (a<b)?(b):(a)
#define swap(x,y) {typeof(y) tmp = y; y = x; x = tmp; }
#define MALLOC(typ) (typ*)malloc(sizeof(typ))
#define CLEAR(co) memset(&co, 0, sizeof(&co))
char* GetCurrentTime();
<<<<<<< HEAD
char* GetCurrentTimeLogFormat();
char* GetTime(time_t *time);

int Random();
void Error(char[]);

#define MAX_CLIENTS MAX_SERVER_COUNT * MAX_USER_COUNT_PER_SERVER
#define MAX_SERVERS MAX_SERVER_COUNT
/*******************************************************/
    #ifndef CHAT_H
    #define CHAT_H
     
    #include <time.h>
     
    #define MAX_SERVER_COUNT 100
    #define MAX_USER_COUNT_PER_SERVER 20
    #define MAX_USER_NAME_LENGTH 16
    #define MAX_ROOM_NAME_LENGTH 16
    #define MAX_USER_LIST_LENGTH 200
     
    #define SEMAPHORE_COUNT 3
    #define SERVER 0
    #define CLIENT 1
    #define LOG 2
     
    #define LOGFILE "/tmp/czat.log"
     
    #define GLOBAL_ROOM_NAME "global"
     
    typedef enum { // typ wiadomości
            MSG_HEARTBEAT = 1,
            MSG_REGISTER,
            MSG_UNREGISTER,
            MSG_JOIN,
            MSG_LIST,
            MSG_LEAVE,
            MSG_STATUS, // nieużywane
            MSG_ROOM,
            MSG_PRIVATE,
            MSG_SERVER,
            MSG_HEARTBEAT_SERVER,
            TERM = 0x7fffffffffffffff // dla ustalenia typu enuma
    } type_t;
     
    typedef struct { // wiadomość klient-klient
            type_t type;
            struct {
                    unsigned int id;
                    time_t send_date;
                    char sender[MAX_USER_NAME_LENGTH];
                    char recipient[MAX_USER_NAME_LENGTH];
                    char message[512];
            } content;
    } standard_message;
     
    typedef struct { // m.in. do potwierdzeń
            type_t type;
            struct {
                    unsigned int id;              
                    char sender[MAX_USER_NAME_LENGTH];
                    int value; //status / numer kolejki
            } content;
    } compact_message;
     
    typedef struct { // lista użytkowników w danym pokoju
            type_t type;
            struct {
                    unsigned int id;              
                    char list[MAX_USER_LIST_LENGTH][MAX_USER_NAME_LENGTH];
            } content;
    } user_list;
     
    typedef struct { // wiadomość serwer-serwer
            type_t type; // always MSG_SERVER
            struct {
                    standard_message msg;
            } content;
    } server_message;
     
    typedef struct { // dane klienta
            int server_queue_key; // server queue key
            int queue_key; // client queue key
            char name[MAX_USER_NAME_LENGTH]; // nazwa usera
            char room[MAX_ROOM_NAME_LENGTH]; // nazwa pokoju
    } client;
     
    typedef struct { // dane servera
            int queue_key; // server queue key
    } server;
     
    typedef struct { // typ segmentu pamięci współdzielonej
            int key_semaphores; // klucz zestawu semaforów
            server servers[MAX_SERVER_COUNT]; // lista serwerów
            client clients[MAX_SERVER_COUNT * MAX_USER_COUNT_PER_SERVER]; // lista klientów
    } shm_type;
     
    #endif // CHAT_H

/******************************************************/
=======
void Error(char[]);

#define SERVER 	0
#define CLIENT 	1
#define LOG		2

/*******************************************************
  * CHAT_DATA_H - BEGIN
  */
	#define MAX_SERVER_COUNT 100
	#define MAX_USER_COUNT_PER_SERVER 20
	#define SEMAPHORE_COUNT 3
	#define MAX_USER_NAME_LENGTH 16
	#define MAX_ROOM_NAME_LENGTH 16
	#define GLOBAL_ROOM_NAME "global"

	typedef enum { // typ wiadomości
	        MSG_HEARTBEAT = 1,
	        MSG_REGISTER,
	        MSG_UNREGISTER,
	        MSG_JOIN,
	        MSG_LIST,
	        MSG_LEAVE,
	        MSG_STATUS,
	        MSG_ROOM,
	        MSG_PRIVATE,
	        MSG_SERVER,
	        TERM = 0x7fffffffffffffff // dla ustalenia typu enuma
	} type_t;
	 
	typedef struct { // wiadomość klient-klient
	        type_t type;
	        struct {
	                unsigned int id;
	                time_t send_date;
	                char sender[MAX_USER_NAME_LENGTH];
	                char recipient[MAX_USER_NAME_LENGTH];
	                char message[512];
	        } content;
	} standard_message;
	 
	typedef struct { // m.in. do potwierdzeń
	        type_t type;
	        struct {
	                unsigned int id;               
	                char sender[MAX_USER_NAME_LENGTH];
	                int value; //status / numer kolejki
	        } content;
	} compact_message;
	 
	typedef struct { // lista pokoi
	        type_t type;
	        struct {
	                unsigned int id;               
	                char list[MAX_USER_COUNT_PER_SERVER * MAX_SERVER_COUNT][MAX_ROOM_NAME_LENGTH];
	        } content;
	} room_list;
	 
	typedef struct { // wiadomość serwer-serwer
	        int type; 
	        standard_message msg;
	} server_message;
	 
	typedef struct { // dane klienta
	        int server_queue_id; // server queue id
	        int queue_id; // client queue id
	        char name[MAX_USER_NAME_LENGTH]; // nazwa usera
	        char room[MAX_ROOM_NAME_LENGTH]; // nazwa pokoju (global)
	} client;

	typedef struct { // dane servera
	        int queue_id; // server queue id
	} server;

	typedef struct { // typ segmentu pamięci współdzielonej
	        int id_semaphores; // id zestawu semaforów
	        server servers[MAX_SERVER_COUNT]; // lista serwerów
	        client clients[MAX_SERVER_COUNT * MAX_USER_COUNT_PER_SERVER]; // lista klientów
	} shm_type;

/**
  * CHAT_DATA_H - END
  ******************************************************/
>>>>>>> master

#endif

/* 

TODO:
<<<<<<< HEAD
 * 
 * heartbeat server	 
 * 
 *
=======
- messageCounter 
- rozpoznawanie /quit itp., niewysylanie samego entera i spacji
- problem z wyswietlaniem 
- powinno byc info o pierwszym wolnym i ostatnim zajetym polu
Q:
- czy mamy zabezpieczyć program przed np. max liczbą sewerów?
- problem co z krzychem..
- co jeśli zamkniemy serwer
- enum zaczynac od 1
- struktura klient numer kolejki powinna miec
>>>>>>> master
*/