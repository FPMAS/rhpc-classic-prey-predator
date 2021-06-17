#ifndef PP_MODEL_H
#define PP_MODEL_H

#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/SharedDiscreteSpace.h"
#include "repast_hpc/GridComponents.h"
#include "repast_hpc/SVDataSet.h"

#include "package.h"
#include "output.h"

class PreyPredatorModel {
	private:
		repast::Properties props;
		unsigned int stop_at;

		unsigned int prey_count;
		unsigned int local_prey_count;
		unsigned int predator_count;
		unsigned int local_predator_count;

		unsigned int grid_width;
		unsigned int grid_height;

		PreyPredatorPackageProvider provider;
		PreyPredatorPackageReceiver receiver;

		repast::SharedContext<PreyPredatorAgent> agent_context;
		Grid<PreyPredatorAgent>* grid;

		repast::SharedContext<Grass> grass_context;
		Grid<Grass>* grass_grid;

		repast::SVDataSet* dataset;

		template<typename T> Grid<T>* build_grid(std::string, boost::mpi::communicator*);
		void init_preys();
		void init_predators();
		void init_grass();

		void load_config();
		void print_config();

	public:
		PreyPredatorModel(
				std::string prop_file, int argc, char** argv,
				boost::mpi::communicator* comm);

		void init();
		void initSchedule(repast::ScheduleRunner& runner);
		
		void move();
		void eat();
		void reproduce();
		void die();
		void grow();

		void synchronize();

		~PreyPredatorModel();
};

template<typename T>
Grid<T>* PreyPredatorModel::build_grid(std::string name, boost::mpi::communicator* comm) {
	auto grid = new repast::SharedDiscreteSpace<T, repast::StrictBorders, repast::SimpleAdder<T>>(name,
			repast::GridDimensions({0, 0}, {(double) grid_width, (double) grid_height}),
			std::vector<int>({1, comm->size()}),
			2, comm);
        std::cout <<
			"RANK " << repast::RepastProcess::instance()->rank() << std::endl <<
			"    GLOBAL BOUNDS: " << grid->bounds().origin() << "->" << grid->bounds().extents()
			<< std::endl <<
			"    LOCAL BOUNDS: " << grid->dimensions().origin() << "->" << grid->dimensions().extents()
			<< std::endl;

		return grid;
}
#endif
