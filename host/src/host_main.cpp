#include <iostream>
#include "host_main.h"

std::string numip_to_stringip(unsigned long long numip) {
	int first = (numip >> 24) & 0xFF;
	int second = (numip >> 16) & 0xFF;
	int third = (numip >> 8) & 0xFF;
	int fourth = numip & 0xFF;
	return std::to_string(first) + '.' + std::to_string(second) + '.' +
		   std::to_string(third) + '.' + std::to_string(fourth);
}

int main(int argc, char** argv)
{
	//Инициализация gpc
	if (argc<2) {
		std::cout << "Использование: host_main <путь к файлу rawbinary>" << '\n';
		return -1;
	}

	//Захват ядра gpc и запись sw_kernel
	gpc* gpc64_inst = new gpc();
	if (gpc64_inst->load_swk(argv[1])==0) {
		std::cout <<"Программное ядро загружено из файла " << argv[1] << '\n';
	}
	else {
		std::cout << "Ошибка загрузки sw_kernel файла << argv[1]" << '\n';
		return -1;
	}

	unsigned long long start_ip = 3272810497ULL;
	gpc64_inst->start(__event__(receive_ips)); //обработчик вставки 
	for (unsigned long long i = 0; i < 254; ++i) {
		gpc64_inst->mq_send(start_ip + i);
	}
	gpc64_inst->sync();

	for (unsigned long long i = 0; i < 254; ++i)  {
		gpc64_inst->start(__event__(get_port_by_ip));
		gpc64_inst->mq_send(start_ip + i);
		unsigned long long port = gpc64_inst->mq_receive();
		std::cout << "port " << port << " for ip " << numip_to_stringip(start_ip + i) << '\n';
	}
	
	return 0;
}

