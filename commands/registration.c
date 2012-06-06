#include <string.h>

#include "../commands.h"
#include "../util.h"
#include "../send.h"

int command_nick(message_callback_data *e)
{
    if (e->message_data->argc < 1)
    {
        command_user_reply(e, "431 :No nickname given");
        return -1;
    }
    if (e->message_data->argv[0][0] == '#' || strcmp(e->message_data->argv[0], "server") == 0)
    {
        command_user_reply(e, "431 :Invalid nickname"); // No proper error code in RFC
        return -1;
    }
    
    if (client_nickname_hashtable_find(e->message_data->argv[0]) != NULL)
    {
        command_user_reply_format(e, ":server 433 %s :Nickname is already in use", e->message_data->argv[0]);
        return -1;
    }
    
    char *oldnick = e->message_data->argv[0];
    if (e->event_data->client->nickname != NULL)
    {
        oldnick = e->event_data->client->nickname;
    }
    command_user_reply_format(e, ":%s NICK %s", oldnick, e->message_data->argv[0]);
    
    if (e->event_data->client->registered == 1)
    {
        /* Inform clients on channels */
        client_channel *c_channel = e->event_data->client->channels;
        send_message_buffer *buffer = NULL;
        while(c_channel != NULL)
        {
            if (buffer == NULL)
            {
                buffer = send_create_buffer_format(":%s NICK %s", oldnick, e->message_data->argv[0]);
            }
            send_enqueue_channel(c_channel->channel, buffer);
            c_channel = c_channel->next;
        }
    }
    else
    {
        e->event_data->client->registered = 1;   
    }
    
    client_set_nickname(e->event_data->client, e->message_data->argv[0]);
    
    return 0;
}

int command_user(message_callback_data *e)
{
    /*if (e->message_data->argc < 1)
    {
        // TODO: Reply system for errors
        return -1;
    }
    if (e->event_data->client->registered != 0)
    {
        // TODO: ^
        return -1;
    }
    
    client_set_username(e->event_data->client, e->message_data->argv[0]);   
    
    // TODO: Also check for reverse order (USER, then NICK)
    if (e->event_data->client->nickname != NULL)
    {
        e->event_data->client->registered = 1;
        info_print_format("Client registered! nick: %s, username: %s", e->event_data->client->nickname, e->event_data->client->username);
    }
    */
    return 0;
}