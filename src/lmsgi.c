#include "lmsgi.h"

void connect_to_pig(lmsgi_state *lmsgi){
    lmsgi->eport = socket(AF_INET, SOCK_STREAM, 0);
    memset(&lmsgi->addr, 0, sizeof(lmsgi->addr));
    lmsgi->addr.sin_family = AF_INET;
    lmsgi->addr.sin_port = htons(lmsgi->sport);
    inet_pton(AF_INET, lmsgi->host, &lmsgi->addr.sin_addr);

    int conn = connect(lmsgi->eport, (struct sockaddr*)&lmsgi->addr, sizeof(lmsgi->addr));
    if(conn < 0){
        fprintf(stderr, "ERROR: <could not connect "
                        "to the pig server at %s:%d\n", lmsgi->host, lmsgi->sport);
        perror("        connect");
        exit(0);
    }

    char buff[200];
    memset(buff, 0, sizeof(buff));
    int written = snprintf(buff, 200, "r%s\r\n", lmsgi->name);

    write(lmsgi->eport, buff, written);
    memset(buff, 0, sizeof(buff));
    int n = read(lmsgi->eport, buff, 200);
    buff[n] = '\0';
    
    if(strcmp(buff,"(OK)\r\n") != 0){
        fprintf(stderr, "ERROR: <could not register to the pig server "
                        "        at %s:%d with instance name '%s'\n"
                        "        received '%s' from server>\n\n", 
                        lmsgi->host, lmsgi->sport, lmsgi->name, buff);
        exit(0);
    }
    lmsgi->inited = 1;
}


#define REG_NODE   'r'
#define REQ_NODE   'g'
#define MSG_NODE   'm'
#define HEY_NODE   'h'
#define SHR_NODE   's'

int msg_pig(lmsgi_state *lmsgi, const char *msg){
    const int len = strlen(msg);
    return write(lmsgi->eport, msg, len);
}

void register_node(const char *msg, char *buffer){
    snprintf(buffer, 200, "%c%s\r\n", REG_NODE, msg);
}

void message_node(const char *line, char *buffer){
    char node[30];
    memset(node, 0, sizeof(node));
    const int len = strlen(line);
    int i = 0;

    while(i < len && i < 30){
        if(line[i] == '!'){
            node[i] = '\0';
            i += 1;
            break;
        }else if(line[i] == ' ')
            continue;
        node[i] = line[i];
        i += 1;
    }
    const char *msg = line+i;
    fprintf(stderr, "Message '%s' to node '%s'\n", msg, node);
    snprintf(buffer, 200, "%c%s|%s\r\n", MSG_NODE, node, msg);
}


void print_usage(lmsgi_state *lmsgi){
    fprintf(stderr,
        "Usage: %s <instance_name> <server_host> <server_port>\n",
        lmsgi->argv[0]);
    exit(EXITED_OK);
}

void lm_init(lmsgi_state *lmsgi, int argc, char **argv){
    lmsgi->argc = argc;
    lmsgi->argv = argv;
    lmsgi->line = 1;
    lmsgi->eport = -1;

    lmsgi->host = NULL;
    lmsgi->name = "nonode";
    lmsgi->sport= 9877; /* default port */
    
    if(argc == 4){
        if(strcmp(argv[1],"--help") == 0){
            print_usage(lmsgi);            
        }
        lmsgi->name = argv[1];
        lmsgi->host = argv[2];
        lmsgi->sport = atoi(argv[3]);
        assert(lmsgi->sport > 0);

    }else{
        print_usage(lmsgi);
    }

    lmsgi->bin_exec = argv[0];
    lmsgi->symbol_table = lm_hashtable_new();
    lmsgi->runtime = lm_new_procvar(lmsgi, "runtime");

    if(lmsgi->host == NULL){
        lmsgi->mail = "nonode";
    }else{
        const int buffsize = sizeof(char) * 101;
        lmsgi->mail = malloc(buffsize);    
        snprintf(lmsgi->mail, buffsize, "%s@%s", lmsgi->name, lmsgi->host);
        if(lmsgi->eport == -1){
            connect_to_pig(lmsgi);
        }else{
            lmsgi->inited = 1;
        }
        
    }
}


void lm_terminate(lmsgi_state *lmsgi, int cleanht){
    if(lmsgi->inited == 1){
        if(strcmp(lmsgi->mail, "nonode") != 0){
            free(lmsgi->mail);
        }
        close(lmsgi->eport);
        if(cleanht){
            lm_hashtable_free(lmsgi->symbol_table);
            lmsgi->symbol_table = NULL;
        }
        lmsgi->inited = 0;     
        lmsgi->argc = -1;
        lmsgi->argv = NULL;
        lmsgi->name = NULL;
        lmsgi->host = NULL;
        lmsgi->eport = -1;
        memset(&lmsgi->addr, 0, sizeof(lmsgi->addr));
        lm_free_object(lmsgi->runtime);
        lmsgi->runtime = NULL;
    }
}

inline lmobject* lm_get_var(lmsgi_state *lmsgi, char *Name){
    lmtentry *Entry = lm_hashtable_get(lmsgi->symbol_table, Name);

    return (Entry == NULL) ? NULL : (lmobject*)Entry->value;
}


inline lmobject*  lm_new_procvar(lmsgi_state *lmsgi, char *Name){
    lmobject *Proc = lm_new_proc();
    lm_hashtable_put(lmsgi->symbol_table, Name, Proc); /* XXX: Lock here */
    return Proc;
}


inline lm_object* lm_new_funvar(lmsgi_state *lmsgi, char *Name, char **Instructions){
    return NULL;
}




