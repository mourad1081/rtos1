FLAGS= -std=c++11 -Wall

all: taskGenerator studyDM simDM

taskGenerator: main.o
	g++ -o

studyDM: main.o
	

simDM: main.o
	
main.o: systemtask.o scheduler.o

systemtask.o: task.o
	g++ $(FLAGS) -c systemtask.cpp

scheduler.o: systemtask.o easyBMP/EasyBMP.o easyBMP/EasyBMP_Font.o
	g++ $(FLAGS) scheduler.cpp
	
task.o:
	g++ $(FLAGS) -c task.cpp



clean:
	rm *.o



g++ $(FLAGS) -DTASK_GENERATOR_MODULE *.cpp -o taskGenerator
g++ $(FLAGS) -DSTUDY_DM_MODULE *.cpp -o studyDM
g++ $(FLAGS) -DSIM_DM_MODULE *.cpp -o simDM


