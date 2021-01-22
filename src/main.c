#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "slog.h"

static const char *filename = "./config/AI_scene.json";


void aiscene_run(const char *scene);

int main(void){
	char buf[512] = {0};
	while(1){
		raw("input scene name: ");
		scanf("%s", buf);
		if(strcmp(buf, "q") == 0){
			exit(0);
		}
		aiscene_run(buf);
	}
	return 0;
}

cJSON *file_read(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		err("%s is not exist", filename);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	if(len <= 0){
		err("%s is empyt", filename);
		goto error;
	}
	char *file_buffer = (char *)malloc(len + 1);
	if(file_buffer == NULL){
		err("system memory is less than %d", len);
		goto error;
	}
	fseek(fp, 0, SEEK_SET);
	fread(file_buffer, len, 1, fp);
	cJSON *root = cJSON_Parse(file_buffer);
	if(root == NULL){
		err("%s is not json", file_buffer);
		goto error;
	}
	fclose(fp);
	return root;

error:
	if(fp){
		fclose(fp);
	}
	return NULL;
}

bool aiscene_selector(cJSON *selector)
{
	if(selector == NULL){
		return false;
	}
	int len = cJSON_GetArraySize(selector);
	int i;
	for(i = 0; i < len; i++){
		cJSON *selector_item = cJSON_GetArrayItem(selector, i);
		if(selector_item == NULL){
			err("selector %d item error", i);
			return false;
		}
		cJSON *extent = cJSON_GetArrayItem(selector_item, "extent");
		if(extent == NULL){
			err("selector item %d has no extent", i);
			return false;
		}

	}
}

void aiscene_run(const char *name)
{
	cJSON *content = file_read(filename);
	if(content == NULL){
		exit(-1);
	}
	cJSON *scene = cJSON_GetObjectItem(content, "scene");
	if(scene == NULL){
		err("%s has no scene", filename);
		exit(-1);
	}
	int scene_array_len = cJSON_GetArraySize(scene);
	dbg("scene number: %d", scene_array_len);
	int i;
	for(i = 0; i < scene_array_len; i++){
		cJSON *scene_item = cJSON_GetArrayItem(scene, i);
		if(scene_item == NULL){
			err("%s scene is not array format", filename);
			exit(-1);
		}
		cJSON *sceneName = cJSON_GetObjectItem(scene_item, "sceneName");
		if(sceneName == NULL){
			err("%d has no %s", i);
			exit(-1);
		}
		if(sceneName->valuestring == name){
			cJSON *condition0 = cJSON_GetObjectItem(scene_item, "condition0");
			if(condition0 == NULL){
				err("scene %s has no condigion0", name);
				return;
			}
			cJSON *selector = cJSON_GetObjectItem(condition0, selector);
			if(selector == NULL){
				wrn("%s variable has no selector", name);
				return;
			}
			bool result = aiscene_selector(selector);
		}
	}
}
