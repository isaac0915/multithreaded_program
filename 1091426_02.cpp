#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include<time.h>
#include <math.h>
#include <iomanip>

using namespace std;


typedef struct str_thdata
{
	int thread_no;
	vector<vector<int>> data_times;
	double avg = 0;
	int num = 0;
} thdata;

void* printHello(void* ptr);

int main(int argc, char* argv[]) {
	//fin1read
	clock_t start, end;
	double cpu_time_used;
	start = clock();
	ifstream fin1;
	string line[1000];
	int line_count = 0;
	fin1.open(argv[1]);
	if (!fin1.is_open()) {
		cout << "Failed to open file.\n";
	}
	while (!fin1.eof()) {
		getline(fin1, line[line_count]);
		//cout << line[line_count].size() << endl;
		line_count++;
	}
	fin1.close();
	/*******字串整理******/
	for (int i = 0; i < line_count; i++) {
		for (int j = 0; j < line[i].size(); j++) {
			if (line[i][j] <= 47 && line[i][j] >= 33 || line[i][j] <= 64 && line[i][j] >= 58) {
				line[i][j] = ' ';
			}
		}
	}

	/*讀取單字*/
	vector<vector<string>> dic;
	vector<string> temp;
	vector<string> temp1;
	string stemp;
	int num = 0;
	for (int i = 0; i < line_count; i++) {
		for (int j = 0; j < line[i].size(); j++) {
			if (i % 2 == 0) {
				num++;
				break;
			}
			else {
				if (line[i][j] != ' ') {
					stemp += line[i][j];
				}
				if (line[i].size() - 1 == j) {
					for (int i = 0; i < stemp.size(); i++) {
						if (stemp[i] <= 57 && stemp[i] >= 48) {
							stemp.clear();
							break;
						}
						else if (i == stemp.size() - 1) {
							temp.push_back(stemp);
							stemp.clear();
						}
					}
				}
				else if (line[i][j] == ' ') {
					for (int i = 0; i < stemp.size(); i++) {
						if (stemp[i] <= 57 && stemp[i] >= 48) {
							stemp.clear();
							break;
						}
					}
					temp.push_back(stemp);
					stemp.clear();
				}
			}
		}
		if (!temp.empty()) {
			dic.push_back(temp);
			temp.clear();
			stemp.clear();
		}
	}

	vector<string> complete_dic;
	complete_dic.push_back(dic[0][0]);
	for (int i = 0; i < dic.size(); i++) {
		for (int j = 0; j < dic[i].size(); j++) {
			for (int k = 0; k < complete_dic.size(); k++) {
				if (complete_dic[k] == dic[i][j]) {
					break;
				}
				else if (k == complete_dic.size() - 1) {
					if (dic[i][j] == "") {
						break;
					}
					complete_dic.push_back(dic[i][j]);
				}
			}

		}
	}
	/*for (int i = 0; i < complete_dic.size(); i++) {
		cout << complete_dic[i] << " ";
	}*/

	/*******算向量*******/
	vector<vector<int>> times(num, vector<int>(complete_dic.size(), 0));
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < complete_dic.size(); j++) {
			if (j != dic[i].size()) {
				for (int k = 0; k < complete_dic.size(); k++) {
					if (dic[i][j].compare(complete_dic[k]) == 0) {
						times[i][k]++;
						break;
					}
				}
			}
			else {
				break;
			}
		}
	}

	/*for (int i = 0; i < num; i++) {
		for (int j = 0; j < complete_dic.size(); j++) {
			cout << times[i][j] << ",";
		}
		cout << endl;
	}*/


	pthread_t threads[num];
	thdata data[num];
	int rc;

	for (int i = 0; i < num; i++) {
		//cout<<"[Main thread]: create TID"<<gettid();
		data[i].data_times = times;
		data[i].thread_no = i;

		rc = pthread_create(&threads[i], NULL, printHello, (void*)&data[i]);
		if (data->thread_no > 10) {
			cout << "[Main thread]:create TID:" << threads[i] << ", DocID:00" << i + 1 << endl;
			//pthread_join(threads[i], NULL);
		}
		else {
			cout << "[Main thread]:create TID:" << threads[i] << ", DocID:000" << i + 1 << endl;
			//pthread_join(threads[i], NULL);
		}
		//pthread_join(threads[i], NULL);
		if (rc) {
			printf("ERORR; return code from pthread_create() is %d\n", rc);
			exit(EXIT_FAILURE);
		}
	}
	/*for (int i = 0; i < num; i++) {
		for (int j = 0; j < complete_dic.size(); j++) {
			cout << data[i].data_times[i][j] << ",";
		}
	cout << endl;
	}*/
	cout << endl;
	for (int i = 0; i < num; i++)
		pthread_join(threads[i], NULL);
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	double com_avg = data[0].avg;
	double com_num = data[0].num;
	for (int i = 0; i < num; i++) {
		if (data[i].avg > com_avg) {
			com_avg = data[i].avg;
			com_num = data[i].num;
		}
	}
	if (com_num + 1 > 9) {
		cout << "[Main thread] KeyDocID:" << "00" << com_num + 1 << " Highest Average Cosine:" << com_avg << endl;
	}
	else {
		cout << "[Main thread] KeyDocID:" << "000" << com_num + 1 << " Highest Average Cosine:" << com_avg << endl;
	}

	cout << "[Main thread]" << " CPU time:" << cpu_time_used * 1000000 << "ms\n";
	pthread_exit(NULL);
}
void* printHello(void* ptr) {
	thdata* data;
	data = (thdata*)ptr;  /* type cast to a pointer to thdata */
	clock_t start, end;
	double cpu_time_used;
	start = clock();
	if (data->thread_no > 9) {
		cout << "[TID=" << pthread_self() << "] DocID:00" << data->thread_no + 1 << "[";
	}
	else {
		cout << "[TID=" << pthread_self() << "] DocID:000" << data->thread_no + 1 << "[";
	}
	//cout << data->data_times[data->thread_no].size();
	for (int i = 0; i < data->data_times[data->thread_no].size(); i++) {
		if (i == data->data_times[data->thread_no].size() - 1) {
			cout << data->data_times[data->thread_no][i] << "]";
		}
		else {
			cout << data->data_times[data->thread_no][i] << ",";
		}

	}
	cout << endl;
	double sum = 0;
	//cout<<data->data_times.size();
	for (int i = 0; i < data->data_times.size(); i++) {
		double cos_k = 0;
		double cos_m1 = 0, cos_m2 = 0;
		double cos = 0;
		long double cos_sim = 0;

		if (i == data->thread_no) {
			continue;
		}
		else {
			for (int j = 0; j < data->data_times[i].size(); j++) {
				//cout<<data->data_times[data->thread_no][j]<<" "<<data->data_times[i][j]<<endl;
				cos_k += data->data_times[data->thread_no][j] * data->data_times[i][j];
				cos_m1 += data->data_times[data->thread_no][j] * data->data_times[data->thread_no][j];
				cos_m2 += data->data_times[i][j] * data->data_times[i][j];
			}
			cos_sim = cos_k / (sqrt(cos_m1) * sqrt(cos_m2));
			if (data->thread_no > 9) {
				if (i + 1 > 9) {
					cout << "[TID=" << pthread_self() << "] cosine(00" << data->thread_no + 1 << ",00" << i + 1 << ")=" << cos_sim << endl;
				}
				else {
					cout << "[TID=" << pthread_self() << "] cosine(00" << data->thread_no + 1 << ",000" << i + 1 << ")=" << cos_sim << endl;
				}

			}
			else {
				if (i + 1 > 9) {
					cout << "[TID=" << pthread_self() << "] cosine(000" << data->thread_no + 1 << ",00" << i + 1 << ")=" << cos_sim << endl;
				}
				else {
					cout << "[TID=" << pthread_self() << "] cosine(000" << data->thread_no + 1 << ",000" << i + 1 << ")=" << cos_sim << endl;
				}
			}


		}
		sum += cos_sim;

		//cout<<cos_k<<" "<<cos_m1<<" "<<cos_m2<<endl;	
	}
	cout << "[TID=" << pthread_self() << "] Avg_cosine:" << setprecision(4) << sum / (data->data_times.size() - 1) << endl;
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "[TID=" << pthread_self() << "]" << " CPU time:" << cpu_time_used * 1000000 << "ms\n";
	//printf("[TID=%lu] CPU time: %dms\n", pthread_self(),cpu_time_used*100000);
	data->avg = sum / (data->data_times.size() - 1);
	data->num = data->thread_no;
	sum = 0;
	cout << endl;
	pthread_exit(NULL);
}
