#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



pthread_t mqtt_thread_pid;


void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	int rc;
	printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		mosquitto_disconnect(mosq);
	}

	rc = mosquitto_subscribe(mosq, NULL, "msgbus/client/get", 1);
    rc = mosquitto_subscribe(mosq, NULL, "msgbus/client/set", 1);
    rc = mosquitto_subscribe(mosq, NULL, "msgbus/client/notify", 1);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_disconnect(mosq);
	}
}

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	int i;
	bool have_subscription = false;

	for(i=0; i<qos_count; i++){
		printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
		if(granted_qos[i] <= 2){
			have_subscription = true;
		}
	}
	if(have_subscription == false){
		fprintf(stderr, "Error: All subscriptions rejected.\n");
		mosquitto_disconnect(mosq);
	}
}
void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	printf("Message with mid %d has been published.\n", mid);
}
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
}

void *mqtt_thread_proc(void *arg)
{
    struct mosquitto *mosq = (struct mosquitto *)arg;
	int rc;


	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_message_callback_set(mosq, on_message);
    mosquitto_publish_callback_set(mosq, on_publish);

	rc = mosquitto_connect(mosq, "localhost", 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

	mosquitto_loop_forever(mosq, -1, 1);
	mosquitto_lib_cleanup();
}

int main(int argc, char const *argv[])
{
    struct mosquitto *mosq = NULL;
	int rc;
	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}
    

    pthread_create(&mqtt_thread_pid, NULL, mqtt_thread_proc, mosq);
    while (1) {
        sleep(2);
        char *payload = "playing";
        mosquitto_publish(mosq, NULL, "msgbus/client/notify", strlen(payload), payload, 2, false);
    }
	return 0;
}
