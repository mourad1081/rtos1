FLAGS= -std=c++11 -Wall
FLAGS_NO_WARNING= -std=c++11

all: taskGenerator studyDM simDM

taskGenerator: mainTASKG.o EasyBMP.o EasyBMP_Font.o EasyBMP_Geometry.o scheduler.o systemtask.o task.o 
	g++ -o taskGenerator $(flAGS) main.o scheduler.o systemtask.o task.o EasyBMP.o EasyBMP_Font.o EasyBMP_Geometry.o

studyDM:       mainSTUDY.o EasyBMP.o EasyBMP_Font.o EasyBMP_Geometry.o scheduler.o systemtask.o task.o
	g++ -o studyDM       $(flAGS) main.o scheduler.o systemtask.o task.o EasyBMP.o EasyBMP_Font.o EasyBMP_Geometry.o

simDM:         mainSIM.o EasyBMP.o EasyBMP_Font.o EasyBMP_Geometry.o scheduler.o systemtask.o task.o
	g++ -o simDM         $(flAGS) main.o scheduler.o systemtask.o task.o EasyBMP.o EasyBMP_Font.o EasyBMP_Geometry.o

mainTASKG.o:
	g++ $(FLAGS) -DTASK_GENERATOR_MODULE -c main.cpp
mainSTUDY.o:
	g++ $(FLAGS) -DSTUDY_DM_MODULE       -c main.cpp
mainSIM.o:
	g++ $(FLAGS) -DSIM_DM_MODULE         -c main.cpp

systemtask.o: task.o
	g++ $(FLAGS) -c systemtask.cpp

scheduler.o: systemtask.o
	g++ $(FLAGS) -c scheduler.cpp

task.o:
	g++ $(FLAGS) -c task.cpp

EasyBMP.o:
	g++ $(FLAGS_NO_WARNING) -c easyBMP/EasyBMP.cpp

EasyBMP_Font.o:
	g++ $(FLAGS_NO_WARNING) -c easyBMP/EasyBMP_Font.cpp

EasyBMP_Geometry.o:
	g++ $(FLAGS_NO_WARNING) -c easyBMP/EasyBMP_Geometry.cpp

clean:
	rm *.o studyDM taskGenerator simDM

