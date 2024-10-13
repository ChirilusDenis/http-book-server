#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "parson.h"


char *HOST = "34.246.184.49";
unsigned short PORT = 8080;

// this function extracts and prints the error passed by the server
void print_error(char *response) {
    char *pos = strstr(response, "{\"error\"");
    JSON_Value *value = json_parse_string(pos);

    printf("ERROR: %s\n", json_object_get_string(json_object(value), "error"));

    json_value_free(value);
}

int main(int argc, char *argv[])
{
    char *message;
    char *line;
    char *response;
    char cookie[1000];
    cookie[0] = 0;
    char library_cookie[1000];
    library_cookie[0] = 0;
    int sockfd;

    char cmd[1000];

    while(1) {
        fgets(cmd, 99, stdin);
        cmd[strlen(cmd) - 1] = 0;
        message = calloc(1000, 1);

        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

        if (strcmp(cmd, "exit") == 0) {
            close_connection(sockfd);
            return 0;
        }

        else if (strcmp(cmd, "register") == 0) {
            char username[50], password[100];
            char *data;
            line = calloc(1000, 1);

            printf("username=");
            fgets(username, 49, stdin);
            username[strlen(username) - 1] = 0;
            printf("password=");
            fgets(password, 49, stdin);
            password[strlen(password) - 1] = 0;
            
            // completion of the HTTP header
            compute_message(message, "POST /api/v1/tema/auth/register HTTP/1.1");
            compute_message(message, "Host: 34.246.184.49:8080");
            compute_message(message, "Content-Type: application/json");

            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);

            // creation of the username and password object
            json_object_set_string(object, "username", username);
            json_object_set_string(object, "password", password);
            data = json_serialize_to_string_pretty(value);

            sprintf(line, "Content-Length: %ld", strlen(data));
            compute_message(message, line);
            strcat(message, "\r\n");

            // pasting message in HTTP message
            strcat(message, data);

            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            // reply of the server contains the word error => server encountered an error
            if (strstr(response, "error") == NULL) {
                printf("SUCCESS\n");
            } else {
                print_error(response);
            }
            free(data);
            free(message);
            free(response);
            free(line);
            continue;
        }

        else if (strcmp(cmd, "login") == 0) {
            char username[50], password[100];
            char *data;
            line = calloc(1000, 1);

            printf("username=");
            fgets(username, 49, stdin);
            username[strlen(username) - 1] = 0;
            printf("password=");
            fgets(password, 49, stdin);
            password[strlen(password) - 1] = 0;
            
            // completion of the HTTP header
            compute_message(message, "POST /api/v1/tema/auth/login HTTP/1.1");
            compute_message(message, "Host: 34.246.184.49:8080");
            compute_message(message, "Content-Type: application/json");

            // creation of the username and password json object
            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);

            json_object_set_string(object, "username", username);
            json_object_set_string(object, "password", password);

            data = json_serialize_to_string_pretty(value);
            sprintf(line, "Content-Length: %ld", strlen(data));
            compute_message(message, line);
            strcat(message, "\r\n");
            
            // pasting data in HTTP message
            strcat(message, data);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            // reply of the server contains the word error => server encountered an error
            if (strstr(response, "error") == NULL) {
                printf("SUCCESS\n");

                // extractio of the sesion cookie
                char *pos = strstr(response, "Set-Cookie:");
                pos += strlen("Set-Cookie: ");  // start of the actual cookie value
                char *end = strchr(pos, ';');
                strncpy(cookie, pos, end-pos);
                cookie[end-pos] = '\0';
            } else {
                print_error(response);
            }

            json_value_free(value);
            free(data);
            free(message);
            free(response);
            free(line);
            continue;
        }

        else if (strcmp(cmd, "logout") == 0) {
            line = calloc(1000, 1);

            // completion of the HTTP header
            compute_message(message, "GET /api/v1/tema/auth/logout HTTP/1.1");
            compute_message(message, "Host: 34.246.184.49:8080");

            strcat(line, "Cookie: ");
            strcat(line, cookie);
            compute_message(message, line);
            strcat(message, "\r\n");

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // reply of the server contains the word error => server encountered an error
            if (strstr(response, "error") == NULL) {
                printf("SUCCESS\n");

                // cookie and access token deletion
                cookie[1] = 0;
                library_cookie[1] = 0;
            } else {
                print_error(response);
            }

            free(response);
            free(message);
            free(line);
            continue;
        }

        else if (strcmp(cmd, "enter_library") == 0) {
            line = calloc(1000, 1);

            // completion of the HTTP header
            compute_message(message, "GET /api/v1/tema/library/access HTTP/1.1");
            compute_message(message, "Host: 34.246.184.49:8080");

            strcat(line, "Cookie: ");
            strcat(line, cookie);
            compute_message(message, line);
            strcat(message, "\r\n");

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // reply of the server contains the word error => server encountered an error
            if (strstr(response, "error") == NULL) {
                printf("SUCCESS\n");

                // extracting the json object containing the library access token
                char *pos = strstr(response, "{\"token\"");
                JSON_Value *value = json_parse_string(pos);
                strcpy(library_cookie, json_object_get_string(json_object(value), "token"));
                json_value_free(value);
            } else {
                print_error(response);
            }

            free(response);
            free(message);
            free(line);
            continue;
        }

        else if (strcmp(cmd, "get_books") == 0) {

            // completion of the HTTP header
            compute_message(message, "GET /api/v1/tema/library/books HTTP/1.1");
            compute_message(message, "Host: 34.246.184.49:8080");

            strcat(message, "Cookie: ");
            compute_message(message, cookie);

            strcat(message, "Authorization: Bearer ");
            compute_message(message, library_cookie);
            strcat(message, "\r\n");

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // reply of the server contains the word error => server encountered an error
            if (strstr(response, "error") == NULL) {
                printf("SUCCESS\n");

                // extraction of the books' data
                // finding the end of the header(start of data) by the double endlines
                char *data = strstr(response, "\r\n\r\n");
                data += 4;
                printf("%s\n", data);
            } else {
                print_error(response);
            }

            free(message);
            free(response);
            continue;
        }

        else if (strcmp(cmd, "get_book") == 0) {
            char id[20];
            printf("id=");
            fgets(id, 19, stdin);
            id[strlen(id) - 1] = 0;

            // completion of the HTTP header
            strcat(message, "GET /api/v1/tema/library/books/");
            strcat(message, id);
            compute_message(message, " HTTP/1.1");
            compute_message(message, "Host: 34.246.184.49:8080");

            strcat(message, "Cookie: ");
            compute_message(message, cookie);

            strcat(message, "Authorization: Bearer ");
            compute_message(message, library_cookie);
            strcat(message, "\r\n");

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // reply of the server contains the word error => server encountered an error
            if (strstr(response, "error") == NULL) {
                printf("SUCCESS\n");

                // extracting book data
                // found after the header. which ends in double endline
                char *data = strstr(response, "\r\n\r\n");
                data += 4;
                printf("%s\n", data);
            } else {
                print_error(response);
            }

            free(message);
            free(response);
            continue;
        }

        else if (strcmp(cmd, "add_book") == 0) {
            char title[100], author[100], genre[100], publisher[100], page_count[100];
            char *data;
            line = calloc(1000, 1);

            fflush(stdin);

            printf("title=");
            fgets(title, 99, stdin);
            title[strlen(title) - 1] = 0;

            printf("author=");
            fgets(author, 99, stdin);
            author[strlen(author) - 1] = 0;

            printf("genre=");
            fgets(genre, 99, stdin);
            genre[strlen(genre) - 1] = 0;

            printf("publisher=");
            fgets(publisher, 99, stdin);
            publisher[strlen(publisher) - 1] = 0;

            printf("page_count=");
            fgets(page_count, 99, stdin);
            page_count[strlen(page_count) - 1] = 0;

            // number of pages provided isn't a number
            if (atoi(page_count) == 0) {
                printf("ERROR: Bad page count\n");
                free(message);
                free(line);
                continue;
            }

            // creation of the object which contains all the book's data
            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);

            json_object_set_string(object, "title", title);
            json_object_set_string(object, "author", author);
            json_object_set_string(object, "genre", genre);
            json_object_set_string(object, "publisher", publisher);
            json_object_set_string(object, "page_count", page_count);

            data = json_serialize_to_string_pretty(value);

            // completion of the HTTP header
            compute_message(message, "POST /api/v1/tema/library/books HTTP/1.1");
            compute_message(message, "Host: 34.246.184.49:8080");
            compute_message(message, "Content-Type: application/json");

            sprintf(line, "Content-Length: %ld", strlen(data));
            compute_message(message, line);

            strcat(message, "Cookie: ");
            compute_message(message, cookie);

            strcat(message, "Authorization: Bearer ");
            compute_message(message, library_cookie);
            strcat(message, "\r\n");

            // adding the book's data at the end
            strcat(message, data);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // reply of the server contains the word error => server encountered an error
            if (strstr(response, "error") == 0) {
                printf("SUCCESS\n");
            } else {
                print_error(response);
            }

            free(message);
            free(data);
            continue;
        }

        else if (strcmp(cmd, "delete_book") == 0) {
            char id[20];

            printf("id=");
            fgets(id, 19, stdin);
            id[strlen(id) - 1] = 0;

            // completion of the HTTP header
            strcat(message, "DELETE /api/v1/tema/library/books/");
            strcat(message, id);
            compute_message(message, " HTTP/1.1");
            compute_message(message, "Host: 34.246.184.49:8080");

            strcat(message, "Cookie: ");
            compute_message(message, cookie);

            strcat(message, "Authorization: Bearer ");
            compute_message(message, library_cookie);
            strcat(message, "\r\n");

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // reply of the server contains the word error => server encountered an error
            if (strstr(response, "error") == NULL) {
                printf("SUCCESS\n");
            } else {
                print_error(response);
            }

            free(message);
            free(response);
            continue;
        }

        else {
            continue;
        }
    }
}
