#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
//#include <windows.h>

long long int meso_used_total = 0, meso_used, meso_used_max = 0, meso_used_min = -1;
unsigned long long int rein_total = 0, destroy_total = 0, rein_count, fail_total = 0, destroy_count, success_total = 0;
long long int rein_max = 0, rein_min = -1, destroy_max = 0, destroy_min = -1;
unsigned int meso_table[] = {164800,328700,492500,656400,820200,984000,1147900,1311700,1475600,1639400,\
6639400,8397300,10422900,12731500,15338200,36514500,43008300,50185100,58072700,66698700,76090000,86273300,97274600,109120000,121834900};
int rein_level, rein_start;
float pot_table[] = {95,90,85,85,80,75,70,65,60,55,50,45,40,35,30,30,30,30,30,30,30,30,3,2,1};
float destroy_table[] = {0,0,0,0,0,0,0,0,0,0,0,0,0.6,1.3,1.4,2.1,2.1,2.1,2.8,2.8,7.0,7.0, 19.4, 29.4, 39.6};
int objective_level, num_repeat, repeat_speed, star_catch;
int rand_var;
char record_1 = 0, record_2 = 0;

void init(int argc) {
	destroy_count = 0;
	rein_count = 0;
	meso_used = 0;
	rein_level = argc > 1 ? rein_start : 0;
}

void update() {
	meso_used_total += meso_used;
	meso_used_max = meso_used_max < meso_used ? meso_used : meso_used_max;
	meso_used_min = meso_used_min > -1 ? 
			meso_used_min > meso_used ? 
				meso_used : meso_used_min
					: meso_used;
	rein_total += rein_count;
	rein_max = rein_max < rein_count ? rein_count : rein_max;
	rein_min = rein_min > -1 ? 
			rein_min > rein_count ? rein_count : rein_min
						: rein_count;
		
	destroy_max = destroy_max < destroy_count ? destroy_count : destroy_max;
	destroy_min = destroy_min > -1 ? 
			destroy_min > destroy_count ? destroy_count : destroy_min
						: destroy_count;				
	destroy_total += destroy_count;
}

int setOptions(int argc, char *argv[]) {
	objective_level = 17;
	num_repeat = 1;
	repeat_speed = 3;
	rein_start = 0;
	
	if(argc == 1)
		printf("이 시뮬레이터는 실행시 옵션을 통해 초기 강화 수치, 도달하고자 하는 강화 수치,\n \
				\r반복 회수, 그리고 시뮬레이션 속도를 조정할 수 있습니다.\n \
				\r옵션을 설정하지 않을 경우, 초기 수치는 0, 목표치는 17성, \n \
				\r반복은 1회 그리고 속도는 3으로 설정됩니다. 자세한 설명은 --help 옵션으로 확인해주세요.\n\
				\r시뮬레이션 중단은 ctrl + c를 통해 가능합니다.\n\n");
				
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "--objective") == 0) {
			if(atoi(argv[++i]) < 1)
				return 0;
				
			objective_level = atoi(argv[i]);
		}
		else if(strcmp(argv[i], "--repeat") == 0) {
			if(atoi(argv[++i]) < 1)
				return 0;
				
			num_repeat = atoi(argv[i]);
		}
		else if(strcmp(argv[i], "--speed") == 0) {
			if(atoi(argv[++i]) < 1)
				return 0;
			
			repeat_speed = atoi(argv[i]);
		}
		else if(strcmp(argv[i], "--start") == 0) {
			if(strcmp(argv[++i],"0") && atoi(argv[i]) < 1)
				return 0;
				
			rein_start = atoi(argv[i]);
		}
		else if(strcmp(argv[i], "--help") == 0) {
			printf("--start 시작레벨 --objective 목표레벨 --repeat 반복회수 --speed 속도 \n \
					\r시작레벨은 0~24, 목표레벨은 1~25, 반복회수는 1 이상, 속도는 1~30으로 설정해주세요.\n\
					\r만약 속도가 31이상으로 설정될 경우, 로그를 출력하지 않으며 빠른 속도로 결과만을 출력합니다.\n\
					\r이는 여러번의 강화를 시뮬레이션하여 통계 수치를 확인하고자 할 때 유용합니다.\n");			
			return 0;
		}
		else
			return 0;
	}		
	if(repeat_speed < 1 || num_repeat < 1 || objective_level < 1 || objective_level > 25 \
		|| rein_level < 0 || objective_level < 1 || rein_level > 24) {
		printf("옵션의 범위를 맞춰주세요.\n");
		return 0;
	}
	return 1;
}

int main(int argc, char *argv[]) {
	if(!setOptions(argc, argv))
		return 0;

	usleep(1000000);	
//	Sleep(1000);	
	
	srand(time(NULL));
	int logging = repeat_speed < 31;
	for(int i = 0; i < num_repeat; i++) {
		init(argc);	
		while(rein_level < objective_level) {
			rein_count++;
			meso_used += meso_table[rein_level];

			rand_var = (rand() % 1000) + 1;
			if(record_1 && record_2) {
				if(logging) printf("chance time!\t");
				rand_var = -1;
			}

			if(pot_table[rein_level] * 10 >= rand_var) {
				success_total++;
				rein_level++;
				record_2 = 0;
				record_1 = 0;
				if(logging) printf("success!\t\t\t\t\t\t\t\t\n");
			} else if((destroy_table[rein_level] + pot_table[rein_level]) * 10 >= rand_var) {
				fail_total++;
				destroy_count++;
				rein_level = 12;
				record_2 = 0;
				record_1 = 0;
				if(logging) printf("destroyed.. \t\t\t\t\t\t\t\t\n");
			} else {
				fail_total++;
				record_2 = record_1;
				record_1 = 0;
				if(rein_level > 10 && rein_level != 15 && rein_level != 20) {
					rein_level--;
					record_1 = 1;	
				}
				if(logging) printf("failed..\t\t\t\t\t\t\t\t\n");
			}
			if(logging) {
				printf("현재 강화 레벨: %d\t", rein_level);
				printf("소모 메소: %lld\t", meso_used);
				printf("파괴 회수: %lld\r", destroy_count);
				usleep(5000000/repeat_speed);
//				Sleep(5000/repeat_speed);
			}
		}
	update();		
	}
	printf("누적 소모 메소 : %lld, 평균: %f\n", meso_used_total, meso_used_total / (float)num_repeat);
	printf("강화 시도 회수: %lld, 평균: %f\n", rein_total, rein_total / (float)num_repeat);
	printf("강화 성공 회수: %lld, 평균: %f\n", success_total, success_total / (float)num_repeat);
	printf("강화 실패 회수: %lld, 평균: %f\n", fail_total, fail_total / (float)num_repeat);
	printf("강화 실패 및 장비 파괴 회수: %lld, 평균: %f\n", destroy_total, destroy_total / (float)num_repeat);	
	
	printf("최소/최대 소모 메소: %lld/%lld\n", meso_used_min, meso_used_max);
	printf("최소/최대 강화 회수: %lld/%lld\n", rein_min, rein_max);
	printf("최소/최대 파괴 회수: %lld/%lld\n", destroy_min, destroy_max);
	return 0;
}
