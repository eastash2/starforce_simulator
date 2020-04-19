#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>  



unsigned int meso_table[] = {164800,328700,492500,656400,820200,984000,1147900,\
1311700,1475600,1639400,6639400,8397300,10422900,12731500,15338200,36514500,\
43008300,50185100,58072700,66698700,76090000,86273300,97274600,109120000,121834900};
int pot_table[] = {95,90,85,85,80,75,70,65,60,55,50,45,40,35,30,30,30,30,30,30,30,30,3,2,1};
float destroy_table[] = {0,0,0,0,0,0,0,0,0,0,0,0,0.6,1.3,1.4,2.1,2.1,2.1,2.8,2.8,7.0,7.0, 19.4, 29.4, 39.6};
int objective_level, repeat_speed, star_catch,\
	eventc, stop_on_destroy, rein_start, thread_num;
long long int num_repeat, procedure = 0;

struct globalStats {
	long long int meso_used_total, meso_used_max, meso_used_min;
	long long int rein_total, destroy_total, fail_total, success_total;
	long long int rein_max, rein_min, destroy_max, destroy_min;
};

struct localStats {
	long long int meso_used, rein_count, destroy_count;
	int rein_level;
};

void initL(struct localStats* l) {
	l->destroy_count = 0;
	l->rein_count = 0;
	l->meso_used = 0;
	l->rein_level = rein_start;
}

void initG(struct globalStats* g) {
	g->meso_used_total = 0;
	g->meso_used_max = 0;
	g->meso_used_min = -1;
	g->rein_total = 0;
	g->rein_max = 0;
	g->rein_min = -1;
	g->destroy_max = 0;
	g->destroy_min = -1;
	g->destroy_total = 0;
	g->fail_total = 0;
	g->success_total = 0;
}

void update(struct localStats* l, struct globalStats* g) {
	g->meso_used_total += l->meso_used;
	g->meso_used_max = g->meso_used_max < l->meso_used ? l->meso_used : g->meso_used_max;
	g->meso_used_min = g->meso_used_min > -1 ? 
			g->meso_used_min > l->meso_used ? 
				l->meso_used : g->meso_used_min
					: l->meso_used;
	g->rein_total += l->rein_count;
	g->rein_max = g->rein_max < l->rein_count ? l->rein_count : g->rein_max;
	g->rein_min = g->rein_min > -1 ? 
			g->rein_min > l->rein_count ? l->rein_count : g->rein_min
						: l->rein_count;
		
	g->destroy_max = g->destroy_max < l->destroy_count ? l->destroy_count : g->destroy_max;
	g->destroy_min = g->destroy_min > -1 ? 
			g->destroy_min > l->destroy_count ? l->destroy_count : g->destroy_min
						: l->destroy_count;				
	g->destroy_total += l->destroy_count;
}

struct globalStats* simulate(int id){
	int rand_var;
	char record_1 = 0, record_2 = 0;
	int logging = repeat_speed < 31;	
	struct globalStats* g = (struct globalStats*) malloc(sizeof(struct globalStats));
	struct localStats* l = (struct localStats*) malloc(sizeof(struct localStats));
	initG(g);
	initL(l);
	srand(time(NULL) + id * id);
	for(int i = 0; i < num_repeat/thread_num; i++) {
		initL(l);	
		while(l->rein_level < objective_level) {
			l->rein_count++;
			l->meso_used += eventc == 2 ? (int)((float)meso_table[l->rein_level] * 0.7f) : meso_table[l->rein_level];
			
			rand_var = (rand() % 1000) + 1;
			if(record_1 && record_2) {
				if(logging) printf("chance time!\t");
				rand_var = -1;
			}
			else if(eventc == 3 && (l->rein_level == 5 || l->rein_level == 10 || l->rein_level == 15))
				rand_var = -1;
				

			if(pot_table[l->rein_level] * 10 >= rand_var) {
				g->success_total++;
				l->rein_level++;
				if(eventc == 1 && l->rein_level < 12)
					l->rein_level++;
				record_2 = 0;
				record_1 = 0;
				if(logging) printf("success!\t\t\t\t\t\t\t\t\n");
			} else if((destroy_table[l->rein_level] + pot_table[l->rein_level]) * 10 >= rand_var) {
				if(logging) printf("destroyed.. at level %d\t\t\t\t\t\t\t\t\n", l->rein_level);
				g->fail_total++;
				l->destroy_count++;
				l->rein_level = 12;
				record_2 = 0;
				record_1 = 0;				
				if(stop_on_destroy) break;
			} else {
				g->fail_total++;
				record_2 = record_1;
				record_1 = 0;
				if(l->rein_level > 10 && l->rein_level != 15 && l->rein_level != 20) {
					l->rein_level--;
					record_1 = 1;	
				}
				if(logging) printf("failed..\t\t\t\t\t\t\t\t\n");
			}
			if(logging) {
				printf("현재 강화 레벨: %d\t", l->rein_level);
				printf("소모 메소: %lld\t", l->meso_used);
				printf("파괴 회수: %lld\r", l->destroy_count);
				Sleep(5000/repeat_speed);
			}
		}
		procedure++;
		update(l, g);		
	}
	free(l);
	if(thread_num > 1)
		_endthreadex((unsigned)g);	
	return g;
}

int setOptions(int argc, char *argv[]) {
	char* opt;
	objective_level = 17;
	num_repeat = 1;
	repeat_speed = 3;
	rein_start = 0;
	eventc = 0;
	stop_on_destroy = 0;
	thread_num = -1;
	
	if(argc == 1)
		printf("이 시뮬레이터는 실행시 옵션을 통해 초기 강화 수치, 도달하고자 하는 강화 수치,\n \
				\r반복 회수, 종료 옵션, 그리고 시뮬레이션 속도를 조정할 수 있습니다.\n \
				\r옵션을 설정하지 않을 경우, 초기 수치는 0, 목표치는 17성, 반복은 1회,\n \
				\r종료 조건은 목표 도달(success), 그리고 속도는 3으로 설정됩니다. 자세한 설명은 --help 옵션으로 확인해주세요.\n\
				\r시뮬레이션 중단은 ctrl + c를 통해 가능합니다.\n\n");
				
	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "--objective")) {
			if(atoi(argv[++i]) < 1)
				return 0;				
			objective_level = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "--repeat")) {
			if(atoi(argv[++i]) < 1)
				return 0;				
			num_repeat = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "--speed")) {
			if(atoi(argv[++i]) < 1)
				return 0;			
			repeat_speed = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "--start")) {
			if(strcmp(argv[++i],"0") && atoi(argv[i]) < 1)
				return 0;				
			rein_start = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "--event")) {
			opt = argv[++i];
			eventc = strcmp(opt, "1+1") ? 
					 strcmp(opt, "30%") ? 
					 strcmp(opt, "100%") ? -1 : 3 : 2 : 1;
		}
		else if(!strcmp(argv[i], "--stop")) {
			opt = argv[++i];
			stop_on_destroy = strcmp(opt, "destroy") ? 
							  strcmp(opt, "success") ? -1 : 0 : 1;
		}
		else if(!strcmp(argv[i], "--thread")) {
			if(strcmp(argv[++i],"0") && atoi(argv[i]) < 1)
				return 0;				
			thread_num = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "--help")) {
			printf("\t--start [시작 레벨] --objective [목표 레벨] --stop [종료 조건]\n\
					\r\t--event [썬데이 이벤트] --repeat [반복 회수] --speed [속도]\n \
					\r\t--thread [스레드 수]\n\n\
					\r\tEx) ~~.exe --objective 22 --stop destroy --event 100%% --speed 31 --repeat 100000\n\n\
					\r 시작 레벨은 0~24, 목표 레벨은 1~25, 반복 회수는 1 이상, 속도는 1~30으로 설정해주세요.\n");
			printf(" 만약 속도가 31이상으로 설정될 경우, 로그를 출력하지 않으며 빠른 속도로 결과만을 출력합니다.\n\
					\r 이는 여러번의 강화를 시뮬레이션하여 통계 수치를 확인하고자 할 때 유용합니다.\n\n\
					\r 종료 조건이 success인 경우, 강화 도중 장비가 파괴되더라도 12성에서 다시 강화를 진행합니다.\n\
					\r 종료 조건이 destroy인 경우, 강화 도중 장비가 파괴 될 경우 다음 강화로 넘어가 시작 레벨에서 다시 시작합니다.\n\n\
					\r 썬데이 이벤트는 1+1(10레벨 이하에서 강화 성공 시 1+1 상승), 100%%(5성, 10성, 15성에서 강화 시도 시 100%% 성공),\n\
					\r 30%%(전체 구간에서 강화 비용 30%% 할인) 셋 중에서 한가지로 설정 할 수 있으며, 명시하지 않을 경우 이벤트는 적용되지 않습니다.\n\n\
					\r 스레드의 수는 20을 넘을 수 없지만 2~5정도를 추천합니다. 그리고 반복 회수가 스레드의 수로 나누어 떨어져야합니다.\n");			
			return 0;
		}
		else
			return 0;
	}
	if(repeat_speed < 31 && thread_num > 1) {
		printf("빠른 속도에서만 멀티스레딩을 지원합니다.\n");
		return 0;
	}
	if(repeat_speed > 30 && num_repeat > 9999 && thread_num == -1) {
		printf("고속 모드에서 많은 시뮬레이션을 빠르게 처리하기 위해 자동으로 멀티스레딩을 설정합니다.\n\
				\r스레드는 4개, 반복 회수는 4의 배수로 재설정됩니다.\n--thread 1 옵션으로 스레드를 하나로 강제할 수 있습니다.\n");
		thread_num = 4;
		num_repeat -= num_repeat % 4;
	}
	thread_num = thread_num == -1 ? 1 : thread_num;
	if(repeat_speed < 1 || num_repeat < 1 || objective_level < 1 || objective_level > 25 \
		|| rein_start < 0 || objective_level < 1 || rein_start > 24 || eventc == -1 || \
		stop_on_destroy == -1 || thread_num < 1 || num_repeat%thread_num != 0 || thread_num > 20) {
		printf("옵션의 범위를 맞춰주세요.\n");
		return 0;
	}
	return 1;
}

void integrateGlobalStats(struct globalStats* tg, struct globalStats* g) {
	g->meso_used_total += tg->meso_used_total;
	g->meso_used_max = g->meso_used_max > tg->meso_used_max ? g->meso_used_max : tg->meso_used_max;
	g->meso_used_min = g->meso_used_min == -1 ? tg->meso_used_min :
			g->meso_used_min < tg->meso_used_min ? g->meso_used_min : tg->meso_used_min;

	g->rein_total += tg->rein_total;
	g->rein_max = g->rein_max > tg->rein_max ? g->rein_max : tg->rein_max;
	g->rein_min = g->rein_min == -1 ? tg->rein_min :
			g->rein_min < tg->rein_min ? g->rein_min : tg->rein_min;

	g->destroy_total += tg->destroy_total;
	g->destroy_max = g->destroy_max > tg->destroy_max ? g->destroy_max : tg->destroy_max;
	g->destroy_min = g->destroy_min == -1 ? tg->destroy_min :
			g->destroy_min < tg->destroy_min ? g->destroy_min : tg->destroy_min;

	g->fail_total += tg->fail_total;
	g->success_total += tg->success_total;

}

int main(int argc, char *argv[]) {
    HANDLE threads[20];  	
	struct globalStats* g;
	struct globalStats* result;
	int iter;
	time_t begin = time(NULL), end;
	if(!setOptions(argc, argv))
		return 0;

	for(int i = 0; i < thread_num; i++)
		threads[i] = (HANDLE)_beginthreadex(NULL, 0, (LPVOID)simulate, (PVOID)i, 0, NULL);		
	
	iter = thread_num;
	while(iter) {
		if(repeat_speed > 30) printf("\rprocedure: %lld%%\t\t", (100*procedure)/(num_repeat));
		if((DWORD)WaitForMultipleObjects(thread_num, threads, 1, 500) != (DWORD)WAIT_TIMEOUT)	
			iter--;
	}
	printf("\rprocedure: 100%%\t\t\n");
	
	g = (struct globalStats*) malloc(sizeof(struct globalStats));
	initG(g);
	for(int i = 0; i < thread_num; i++) {			
		GetExitCodeThread(threads[i],(unsigned *)&result);
		integrateGlobalStats(result, g);
		CloseHandle(threads[i]);
		free(result);
	}

	Sleep(500);
	printf("누적 소모 메소 : %lld, 평균: %f\t\t\t\t\n", g->meso_used_total, g->meso_used_total / (float)num_repeat);
	printf("강화 시도 회수: %lld, 평균: %f\n", g->rein_total, g->rein_total / (float)num_repeat);
	printf("강화 성공 회수: %lld, 평균: %f\n", g->success_total, g->success_total / (float)num_repeat);
	printf("강화 실패 회수: %lld, 평균: %f\n", g->fail_total, g->fail_total / (float)num_repeat);
	printf("강화 실패 및 장비 파괴 회수: %lld, 평균: %f\n", g->destroy_total, g->destroy_total / (float)num_repeat);	
	
	printf("최소/최대 소모 메소: %lld/%lld\n", g->meso_used_min, g->meso_used_max);
	printf("최소/최대 강화 회수: %lld/%lld\n", g->rein_min, g->rein_max);
	printf("최소/최대 파괴 회수: %lld/%lld\n", g->destroy_min, g->destroy_max);
	
	end = time(NULL);
	printf("경과 시간: %lldsec\n", end - begin);
	return 0;
}
