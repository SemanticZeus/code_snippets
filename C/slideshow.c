//libs: -DSDL_DISABLE_IMMINTRIN_H `pkg-config --libs --cflags  sdl2 SDL2_image`
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

#define  num_images  200000
int hist = num_images/2;
int tresh = .02*num_images;
int read_image_finished = 0;
int end = 0;
char *filename;

int num_par_folders = 10;
int active_folder = 1;
//char image_list[num_par_folders][num_images][2048];
char image_list[num_images][2048];
int r_index,w_index;
int start_index;
SDL_mutex *mutex;
SDL_cond *image_list_not_full_cond;
SDL_cond *image_list_not_empty_cond;

int is_list_full()
{
	return (w_index == start_index-1 || (w_index == num_images-1 && start_index == 0));
}

int is_list_empty()
{
	return start_index == w_index;
}

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1000
char folders_list[MAX_LINES][MAX_LINE_LENGTH];
int num_folders = 0;
int skip_folder = 0;

int read_folders(const char *f)
{
	FILE *file = fopen("images.list", "r");
	if (file == NULL) return 0;
	while (num_folders<MAX_LINES) {
		if (fgets(folders_list[num_folders], MAX_LINE_LENGTH, file)==NULL) break;
		int len = strlen(folders_list[num_folders]);
		for (int i=0; i<len; i++)
			if (folders_list[num_folders][i]=='\n' || folders_list[num_folders][i]=='\r')
				folders_list[num_folders][i]='\0';
		//if (folders_list[num_folders][len]=='\n') folders_list[num_folders][len]='\0';
		num_folders++;
	}
	return num_folders;
}

int isImage(const char* filename)
{
	int isImage = 0;
	const char *imageSuffix[] = {".jpg", ".jpeg", ".png"};
	int fl = strlen(filename);
	for (int i=0; i<3; i++) {
		int sl = strlen(imageSuffix[i]);
		if (fl>sl && strcmp(filename+fl-sl,imageSuffix[i]) == 0) {
			isImage = 1;
			break;
		}
	}
	return isImage;
}

void read_image_recursive(const char* current_dir)
{
	int paths_index=0;
	DIR *dir;
	char *paths[2048];
	struct dirent *entry;
	if (!(dir = opendir(current_dir)))
		return;
	int cd_len = strlen(current_dir);
	while ((entry = readdir(dir)) != NULL && !end && !skip_folder) {
		if (entry->d_type == DT_DIR || entry->d_type == DT_LNK) {
			char path[2048];
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			snprintf(path, sizeof(path), "%s/%s", current_dir, entry->d_name);
			int len=strlen(path);
			if (paths_index>=2048) continue;
			paths[paths_index]=(char *) malloc((len+1)*sizeof(char));
			strcpy(paths[paths_index], path);
			paths_index++;
		} else {
			//printf("image name = %s\n", entry->d_name);
			if (!isImage(entry->d_name)) continue;
			SDL_LockMutex(mutex);
			while (is_list_full() && !end) {
				SDL_CondWait(image_list_not_full_cond, mutex);
			}
			snprintf(image_list[w_index], sizeof(image_list[w_index]),
			         "%s/%s", current_dir, entry->d_name);
			if (++w_index == num_images) w_index = 0;
			SDL_CondSignal(image_list_not_empty_cond);
			SDL_UnlockMutex(mutex);
		}
	}
	for (int i=0; i<paths_index; i++) {
		read_image_recursive(paths[i]);
		free(paths[i]);
	}
	closedir(dir);
}

int free_space()
{
	if (w_index < start_index) return (start_index-w_index);
	else return num_images + start_index-w_index;
}

int read_images_thread(void* ptr)
{
	if (num_folders==0) return 0;
	for (int i=0; !end; i=(i+1)%num_folders) {
		//printf("%s\n", folders_list[i]);
		skip_folder = 0;
		read_image_recursive(folders_list[i]);
	}
	read_image_finished=1;
	SDL_CondSignal(image_list_not_empty_cond);
	return 0;
}

int advance_r_index(int advance)
{
	if (advance==0) return r_index;
	if (advance<0 && r_index>start_index) {
		int t = r_index+advance;
		if (t<start_index) t=start_index;
		return t;
	} else if (advance<0 && r_index<start_index) {
		int t=r_index+advance;
		if (t<0) {
			t+=num_images;
			if (t<start_index) t=start_index;
		}
		return t;
	} else if (advance>0 && r_index<w_index) {
		int t=r_index+advance;
		if (t>=w_index) t=w_index-1;
		return t;
	} else if (advance>0 && r_index>w_index) {
		int t=r_index+advance;
		if (t>=num_images) {
			t-=num_images;
			if (t>=w_index) t=w_index-1;
		}
		return t;
	}
	return r_index;
}

void getFilename_v2(int advance)
{
	//if (read_image_finished) return;
	if (is_list_empty()) return;
	SDL_LockMutex(mutex);
	r_index = advance_r_index(advance);
	filename = image_list[r_index];
	if (free_space() < tresh &&
	        ((r_index-start_index>hist && r_index>start_index) ||
	         (r_index < start_index && r_index-hist+num_images>start_index))) {
		start_index+=tresh;
		if (start_index>=num_images) start_index-=num_images;
	}
	SDL_CondSignal(image_list_not_full_cond);
	SDL_UnlockMutex(mutex);
}

void terminate()
{
	end=1;
	SDL_LockMutex(mutex);
	SDL_CondSignal(image_list_not_full_cond);
	SDL_CondSignal(image_list_not_empty_cond);
	SDL_UnlockMutex(mutex);
}

int main(int argc, char *argv[]) {
	int flag = SDL_WINDOW_RESIZABLE;
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *bitmapTex = NULL;
	SDL_Surface *bitmapSurface = NULL;
	SDL_Thread *thread = NULL;

	mutex = SDL_CreateMutex();
	image_list_not_empty_cond = SDL_CreateCond();
	image_list_not_full_cond = SDL_CreateCond();

	int posX = 100, posY = 100, width = 800, height = 600;

	SDL_Init(SDL_INIT_VIDEO);

	read_folders("images.list");
	char *folder="";
	if (num_folders) folder = folders_list[0];
	printf("%s num_folders = %d\n", folders_list[0], num_folders);
	win = SDL_CreateWindow(filename, posX, posY, width, height, flag);
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	//thread = SDL_CreateThread(read_image_thread, "ImageThread", (void *)folder);
	thread = SDL_CreateThread(read_images_thread, "ImageThread", (void *)folder);

	int w,h,iw,ih;
	int update_image = 1;
	int next_image = 0;
	int load_image = 1;
	int duration = 2000;
	int passed_time = SDL_GetTicks();
	int mouse_visible = 1;
	int mouse_timer = SDL_GetTicks();
	while (1) {
		int delay=0;
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			delay=0;
			if (e.type == SDL_QUIT ||
			        (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
				terminate();
				break;
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_f) {
				flag ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
				SDL_SetWindowFullscreen(win, flag);
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT) {
				next_image = 1;
				SDL_FlushEvent(SDL_KEYDOWN);
				load_image = 1;
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
				duration+=500;
				printf("duration = %d\n", duration);
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
				duration-=500;
				printf("duration = %d\n", duration);
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT) {
				next_image = -1;
				SDL_FlushEvent(SDL_KEYDOWN);
				load_image = 1;
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) {
				next_image = 1;
				load_image = 1;
				SDL_FlushEvent(SDL_KEYDOWN);
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHTBRACKET) {
				//printf("fast forward\n");
				next_image = 200;
				load_image = 1;
				if (e.key.keysym.mod & KMOD_GUI) next_image=1000;
				SDL_FlushEvent(SDL_KEYDOWN);
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_s) {
				skip_folder = 1;
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFTBRACKET) {
				next_image = -200;
				load_image = 1;
				SDL_FlushEvent(SDL_KEYDOWN);
			}
			if (e.type == SDL_MOUSEMOTION) {
				mouse_visible = 1;
				mouse_timer = SDL_GetTicks();
				SDL_ShowCursor(SDL_ENABLE);
			}
			if (e.type == SDL_WINDOWEVENT)
				update_image = 1;
		} else {
			delay=20;
		}
		if (passed_time + duration < SDL_GetTicks()) {
			next_image = 1;
			load_image = 1;
			passed_time = SDL_GetTicks();
		}
		if (mouse_visible && mouse_timer+500<SDL_GetTicks()) {
			mouse_visible=0;
			SDL_ShowCursor(SDL_DISABLE);
		}
		if (load_image) {
			getFilename_v2(next_image);
			//printf("filename = %s\n", filename);
			SDL_DestroyTexture(bitmapTex);
			if (filename) {
				bitmapSurface = IMG_Load(filename);
			} else {
				//printf("Error getting new filename\n");
				continue;
			}
			if (bitmapSurface == NULL) {
				//printf("Error loading image %s\n", filename);
				next_image=0;
				continue;
			}
			bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
			SDL_FreeSurface(bitmapSurface);
			if (bitmapTex == NULL) {
				printf("Error opening texture file\n");
				//condition=1;
				continue;
			}
			SDL_QueryTexture(bitmapTex, NULL, NULL, &iw, &ih);
			//printf("width = %d, height = %d\n", iw, ih);
			update_image=1;
			load_image = 0;
			next_image=0;
			passed_time = SDL_GetTicks();
		}
		if (update_image) {
			SDL_GetWindowSize(win,&w,&h);
			SDL_Rect dstrect;
			double s = (double)w/iw;
			if (s*ih > h) s = (double)h/ih;
			dstrect.x = (w-s*iw)/2;
			dstrect.y = (h-s*ih)/2;
			dstrect.w = s*iw;
			dstrect.h = s*ih;

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, bitmapTex, NULL, &dstrect);
			SDL_RenderPresent(renderer);
			update_image=0;
		}
		SDL_Delay(delay);
	}
	SDL_WaitThread(thread, NULL);
	SDL_DestroyTexture(bitmapTex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

	SDL_Quit();

	return 0;
}
