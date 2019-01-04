#include <string.h>

//#include "emulator.h"
#include "display.h"
#include "ui_dialog.h"
#include <dirent.h>
#include <ctype.h>

#include "esp_partition.h"
#include "esp_ota_ops.h"

#include "odroid_settings.h"


enum
{
	NINTENDO_ES = 0,
	GAME_BOY,
	GAME_BOY_COLOR,
	SEGA_MS,
	GAME_GEAR,
	COLECO_V,
};


char** files;
int emulator;

void odroid_system_application_set(int slot)
{
    const esp_partition_t* partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_APP,
        ESP_PARTITION_SUBTYPE_APP_OTA_MIN + slot,
        NULL);
    if (partition != NULL)
    {
        esp_err_t err = esp_ota_set_boot_partition(partition);
        if (err != ESP_OK)
        {
            printf("odroid_system_application_set: esp_ota_set_boot_partition failed.\n");
            abort();
        }
    }
}

inline static void swap(char** a, char** b)
{
    char* t = *a;
    *a = *b;
    *b = t;
}

static int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++)
    {
        int d = tolower((int)*a) - tolower((int)*b);
        if (d != 0 || !*a) return d;
    }
}


static int partition (char* arr[], int low, int high)
{
    char* pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high- 1; j++)
    {
        if (strcicmp(arr[j], pivot) < 0)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

static void quick_sort(char* arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);

        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

static void sort_files(char** files, int count)
{
    if (count > 1)
    {
        quick_sort(files, 0, count - 1);
    }
}

int get_files_list(const char* path, const char* extension, char*** filesOut)
{
    const int MAX_FILES = 1024;

    int count = 0;
    char** result = (char**)malloc(MAX_FILES * sizeof(void*));
    if (!result) abort();


    DIR *dir = opendir(path);
    if( dir == NULL )
    {
        printf("opendir failed.\n");
        //abort();
        return 0;
    }

    int extensionLength = strlen(extension);
    if (extensionLength < 1) abort();


    char* temp = (char*)malloc(extensionLength + 1);
    if (!temp) abort();

    memset(temp, 0, extensionLength + 1);


    struct dirent *entry;
    while((entry=readdir(dir)) != NULL)
    {
        size_t len = strlen(entry->d_name);


        // ignore 'hidden' files (MAC)
        bool skip = false;
        if (entry->d_name[0] == '.') skip = true;


        memset(temp, 0, extensionLength + 1);
        if (!skip)
        {
            for (int i = 0; i < extensionLength; ++i)
            {
                temp[i] = tolower((int)entry->d_name[len - extensionLength + i]);
            }

            if (len > extensionLength)
            {
                if (strcmp(temp, extension) == 0)
                {
                    result[count] = (char*)malloc(len + 1);
                    //printf("%s: allocated %p\n", __func__, result[count]);

                    if (!result[count])
                    {
                        abort();
                    }

                    strcpy(result[count], entry->d_name);
                    ++count;

                    if (count >= MAX_FILES) break;
                }
            }
        }
    }

    closedir(dir);
    free(temp);

    sort_files(result, count);

    *filesOut = result;
    return count;
}

void loader(ui_list_item_t *item, void *arg) {
	char* fileName = (char*)arg;
	char* path = (char *)malloc(sizeof(char)*100);
	switch(emulator) {
		case NINTENDO_ES:
			strcpy(path, "/sd/roms/nes/");
			strcat(path, fileName);
			odroid_settings_RomFilePath_set(path);
			odroid_system_application_set(1);
			esp_restart();
			break;
		case GAME_BOY:
			strcpy(path, "/sd/roms/gb/");
			strcat(path, fileName);
			odroid_settings_RomFilePath_set(path);
			odroid_system_application_set(2);
			esp_restart();
			break;
		case GAME_BOY_COLOR:
			strcpy(path, "/sd/roms/gbc/");
			strcat(path, fileName);
			odroid_settings_RomFilePath_set(path);
			odroid_system_application_set(2);
			esp_restart();
			break;
		case SEGA_MS:
			strcpy(path, "/sd/roms/sms/");
			strcat(path, fileName);
			odroid_settings_RomFilePath_set(path);
			odroid_system_application_set(3);
			esp_restart();
			break;
		case GAME_GEAR:
			strcpy(path, "/sd/roms/gg/");
			strcat(path, fileName);
			odroid_settings_RomFilePath_set(path);
			odroid_system_application_set(3);
			esp_restart();
			break;
		case COLECO_V:
			strcpy(path, "/sd/roms/col/");
			strcat(path, fileName);
			odroid_settings_RomFilePath_set(path);
			odroid_system_application_set(3);
			esp_restart();
			break;
		default: break;
	}
	free(path);
}

void file_picker(ui_list_item_t *item, void *arg) {
	emulator = (int) arg;
	char* emu_name = "";
	char* extension = "";
	char* path = "";
	switch(emulator) {
		case NINTENDO_ES:
			path = "/sd/roms/nes";
			emu_name = "NES";
			extension = ".nes";
			break;
		case GAME_BOY:
			path = "/sd/roms/gb";
			emu_name = "Game Boy";
			extension = ".gb";
			break;
		case GAME_BOY_COLOR:
			path = "/sd/roms/gbc";
			emu_name = "Game Boy Color";
			extension = ".gbc";
			break;
		case SEGA_MS:
			path = "/sd/roms/sms";
			emu_name = "Sega MS";
			extension = ".sms";
			break;
		case GAME_GEAR:
			path = "/sd/roms/gg";
			emu_name = "Game Gear";
			extension = ".gg";
			break;
		case COLECO_V:
			path = "/sd/roms/col";
			emu_name = "ColecoVision";
			extension = ".col";
			break;
		default: break;
	}

	rect_t r = {
		.x = fb->width/2 - 300/2,
		.y = fb->height/2 - 180/2,
		.width = 300,
		.height = 200,
	};
	ui_dialog_t *d = ui_dialog_new(item->list->d, r, emu_name);

	rect_t lr = {
		.x = 0,
		.y = 0,
		.width = d->cr.width,
		.height = d->cr.height,
	};
	ui_list_t *list = ui_dialog_add_list(d, lr);

	int count = get_files_list(path, extension, &files);
	for (int i = 0; i < count; i++)
		ui_list_append_text(list, files[i], loader, (void *)files[i]);

	ui_dialog_showmodal(d);
	ui_dialog_destroy(d);
}

void emulator_list_dialog(ui_list_item_t *item, void *arg) {
	rect_t r = {
		.x = fb->width/2 - 310/2,
		.y = fb->height/2 - 200/2,
		.width = 310,
		.height = 215,
	};
	ui_dialog_t *d = ui_dialog_new(item->list->d, r, "Emulators");

	rect_t lr = {
		.x = 0,
		.y = 0,
		.width = d->cr.width,
		.height = d->cr.height,
	};

	ui_list_t *list = ui_dialog_add_list(d, lr);
	ui_list_append_text(list, "Nintendo Entartainment System", file_picker, (void *)NINTENDO_ES);
	ui_list_append_text(list, "Game Boy", file_picker, (void *)GAME_BOY);
	ui_list_append_text(list, "Game Boy Color", file_picker, (void *)GAME_BOY_COLOR);
	ui_list_append_text(list, "Sega Master System", file_picker, (void *)SEGA_MS);
	ui_list_append_text(list, "Game Gear", file_picker, (void *)GAME_GEAR);
	ui_list_append_text(list, "ColecoVision", file_picker, (void *)COLECO_V);
	ui_dialog_showmodal(d);
	ui_dialog_destroy(d);
}
