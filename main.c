#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(void) {
	long long int meso_used_total = 0, meso_used = 0, meso_used_max = 0, meso_used_min = -1;
	unsigned long long int rein_total = 0, destroy_total = 0, rein_count = 0, fail_total = 0, destroy_count = 0, success_total = 0;
	long long int rein_max = 0, rein_min = -1, destroy_max = 0, destroy_min = -1;
	unsigned int meso_table[] = {164800,328700,492500,656400,820200,984000,1147900,1311700,1475600,1639400,6639400,8397300,10422900,12731500,15338200,36514500,43008300,50185100,58072700,66698700,76090000,86273300,97274600,109120000,121834900};
	int rein_level = 0;
	float pot_table[] = {95,90,85,85,80,75,70,65,60,55,50,45,40,35,30,30,30,30,30,30,30,30,3,2,1};
	float destroy_table[] = {0,0,0,0,0,0,0,0,0,0,0,0,0.6,1.3,1.4,2.1,2.1,2.1,2.8,2.8,7.0,7.0, 19.4, 29.4, 39.6};
	int objective_level, num_repeat, repeat_speed, star_catch;
	int rand_var;
	char record_1 = 0, record_2 = 0;
	
	
	printf("도달하고자 하는 강화 수치를 입력하세요(1 이상 25 이하): ");
	scanf("%d", &objective_level);
	printf("반복 회수를 입력하세요(1 이상): ");
	scanf("%d", &num_repeat);
	printf("강화 속도를 입력하세요(1~15: 눈에 보임, 16이상: 매우 빠른 속도, 로그 스킵): ");
	scanf("%d", &repeat_speed);
	if(repeat_speed < 1 || num_repeat < 1 || objective_level < 1 || objective_level > 25) {
		printf("입력 수치의 범위를 맞춰주세요.\n");
		return 0;
	}

	
	srand(time(NULL));
	if(repeat_speed < 16)
		for(int i = 0; i < num_repeat; i++) {
			while(rein_level < objective_level) {
				rein_count++;
				meso_used += meso_table[rein_level];

				rand_var = (rand() % 1000) + 1;
				if(record_1 && record_2) {
					printf("chance time!\t");
					rand_var = -1;
				}

				if(pot_table[rein_level] * 10 >= rand_var) {
					success_total++;
					rein_level++;
					record_2 = 0;
					record_1 = 0;
					printf("success!\t\t\t\t\t\t\n");
				} else if((destroy_table[rein_level] + pot_table[rein_level]) * 10 >= rand_var) {
					fail_total++;
					destroy_count++;
					rein_level = 12;
					record_2 = 0;
					record_1 = 0;
					printf("destroyed.. \t\t\t\t\t\t\n");
				} else {
					fail_total++;
					record_2 = record_1;
					record_1 = 0;
					if(rein_level > 10 && rein_level != 15 && rein_level != 20) {
						rein_level--;
						record_1 = 1;	
					}
					printf("failed..\t\t\t\t\t\t\n");
				}
				printf("현재 강화 레벨: %d\t", rein_level);
				printf("소모 메소 : %lld\r", meso_used);
				usleep(5000000/repeat_speed);
			}
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

			destroy_count = 0;
			rein_count = 0;
			meso_used = 0;
			rein_level = 0;
		}
	else
		for(int i = 0; i < num_repeat; i++) {
			while(rein_level < objective_level) {
				rein_count++;
				meso_used += meso_table[rein_level];

				rand_var = (rand() % 1000) + 1;
				if(record_1 && record_2)
					rand_var = -1;

				if(pot_table[rein_level] * 10 >= rand_var) {
					success_total++;
					rein_level++;
					record_2 = 0;
					record_1 = 0;
				} else if((destroy_table[rein_level] + pot_table[rein_level]) * 10 >= rand_var) {
					fail_total++;
					destroy_count++;
					rein_level = 12;
					record_2 = 0;
					record_1 = 0;
				} else {
					fail_total++;
					record_2 = record_1;
					record_1 = 0;
					if(rein_level > 10 && rein_level != 15 && rein_level != 20) {
						rein_level--;
						record_1 = 1;
					}
				}
			}
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
		
			destroy_total += destroy_count;
			destroy_max = destroy_max < destroy_count ? destroy_count : destroy_max;
			destroy_min = destroy_min > -1 ? 
				destroy_min > destroy_count ? destroy_count : destroy_min
							: destroy_count;				

			destroy_count = 0;
			rein_count = 0;
			meso_used = 0;
			rein_level = 0;
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
