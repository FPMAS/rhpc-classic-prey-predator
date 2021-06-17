#include "model.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/SVDataSetBuilder.h"
#include "types.h"

void PreyPredatorModel::load_config() {
	Prey::init_energy = repast::strToInt(props.getProperty("prey.init_energy"));
	Prey::energy_gain = repast::strToInt(props.getProperty("prey.energy_gain"));
	Prey::move_cost = repast::strToInt(props.getProperty("prey.move_cost"));
	Prey::reproduction_rate = repast::strToDouble(props.getProperty("prey.reproduction_rate"));

	Predator::init_energy = repast::strToInt(props.getProperty("predator.init_energy"));
	Predator::energy_gain = repast::strToInt(props.getProperty("predator.energy_gain"));
	Predator::move_cost = repast::strToInt(props.getProperty("predator.move_cost"));
	Predator::reproduction_rate = repast::strToDouble(props.getProperty("predator.reproduction_rate"));

	Grass::growing_rate = repast::strToInt(props.getProperty("grass.growing_rate"));
}

void PreyPredatorModel::print_config() {
	std::cout << "Current configuration:" << std::endl;
	std::cout << "Grid::width=" << grid_width << std::endl;
	std::cout << "Grid::height=" << grid_height << std::endl;

	std::cout << "Prey::count=" << prey_count << std::endl;
	std::cout << "Prey::init_energy=" << Prey::init_energy << std::endl;
	std::cout << "Prey::energy_gain=" << Prey::energy_gain << std::endl;
	std::cout << "Prey::move_cost=" << Prey::move_cost << std::endl;
	std::cout << "Prey::reproduction_rate=" << Prey::reproduction_rate << std::endl;

	std::cout << "Predator::count=" << predator_count << std::endl;
	std::cout << "Predator::init_energy=" << Predator::init_energy << std::endl;
	std::cout << "Predator::energy_gain=" << Predator::energy_gain << std::endl;
	std::cout << "Predator::move_cost=" << Predator::move_cost << std::endl;
	std::cout << "Predator::reproduction_rate=" << Predator::reproduction_rate << std::endl;

	std::cout << "Grass::growing_rate=" << Grass::growing_rate << std::endl;

	std::cout << "Stop at: " << stop_at << std::endl;
}

PreyPredatorModel::PreyPredatorModel(
		std::string prop_file, int argc, char** argv,
		boost::mpi::communicator* comm) :
	agent_context(comm), grass_context(comm),
	props(prop_file, argc, argv, comm),
	stop_at(repast::strToInt(props.getProperty("stop.at"))),
	prey_count(repast::strToInt(props.getProperty("prey.count"))),
	local_prey_count(prey_count/comm->size()),
	predator_count(repast::strToInt(props.getProperty("predator.count"))),
	local_predator_count(predator_count/comm->size()),
	grid_width(repast::strToInt(props.getProperty("grid.width"))),
	grid_height(repast::strToInt(props.getProperty("grid.height"))),
	provider(&agent_context), receiver(&agent_context) {
		load_config();
		print_config();

		grid = build_grid<PreyPredatorAgent>("PreyGrid", comm);   
		agent_context.addProjection(grid);

		grass_grid = build_grid<Grass>("GrassGrid", comm);
		grass_context.addProjection(grass_grid);

		initializeRandom(props, comm);

		// Data collection
		// Creates the data set builder
		std::string output_file("./model.csv");
		repast::SVDataSetBuilder builder(
				output_file.c_str(), ",",
				repast::RepastProcess::instance()->getScheduleRunner().schedule()
				);

		// Create the individual data sets to be added to the builder
		PreyDataSource* prey_data_source = new PreyDataSource(agent_context);
		builder.addDataSource(createSVDataSource(
					"prey", prey_data_source, std::plus<int>()
					));

		PredatorDataSource* predator_data_source = new PredatorDataSource(agent_context);
		builder.addDataSource(createSVDataSource(
					"predator", predator_data_source, std::plus<int>()
					));

		GrassDataSource* grass_data_source = new GrassDataSource(grass_context);
		builder.addDataSource(createSVDataSource(
					"grass", grass_data_source, std::plus<int>()
					));

		// Use the builder to create the data set
		dataset = builder.createDataSet();
	}

void PreyPredatorModel::init_preys() {
	int rank = repast::RepastProcess::instance()->rank();

	repast::IntUniformGenerator gen_x = 
		repast::Random::instance()->createUniIntGenerator(
				grid->dimensions().origin().getX(),
				grid->dimensions().origin().getX() + grid->dimensions().extents().getX()-1
				);
	repast::IntUniformGenerator gen_y = 
		repast::Random::instance()->createUniIntGenerator(
				grid->dimensions().origin().getY(),
				grid->dimensions().origin().getY() + grid->dimensions().extents().getY()-1
				);

	for(int i = 0; i < local_prey_count; i++) {
		repast::AgentId id(Prey::current_prey_id++, rank, PREY);
		id.currentRank(rank);

		repast::Point<int> initial_location(gen_x.next(), gen_y.next());

		Prey* prey = new Prey(id);
		agent_context.addAgent(prey);
		grid->moveTo(prey, initial_location);
		std::cout << "Init Prey " << id << " at " << initial_location
			<< std::endl;
	}
}

void PreyPredatorModel::init_predators() {
	int rank = repast::RepastProcess::instance()->rank();

	repast::IntUniformGenerator gen_x = 
		repast::Random::instance()->createUniIntGenerator(
				grid->dimensions().origin().getX(),
				grid->dimensions().origin().getX() + grid->dimensions().extents().getX()-1
				);
	repast::IntUniformGenerator gen_y = 
		repast::Random::instance()->createUniIntGenerator(
				grid->dimensions().origin().getY(),
				grid->dimensions().origin().getY() + grid->dimensions().extents().getY()-1
				);

	for(int i = 0; i < local_predator_count; i++) {
		repast::AgentId id(Predator::current_predator_id++, rank, PREDATOR);
		id.currentRank(rank);

		repast::Point<int> initial_location(gen_x.next(), gen_y.next());

		Predator* predator = new Predator(id);
		agent_context.addAgent(predator);
		grid->moveTo(predator, initial_location);
		std::cout << "Init Predator " << id << " at " << initial_location
			<< std::endl;
	}
}

void PreyPredatorModel::init_grass() {
	int rank = repast::RepastProcess::instance()->rank();
	float init_grass_rate = repast::strToDouble(props.getProperty("grass.init_rate"));
	Grass::growing_rate = repast::strToInt(props.getProperty("grass.growing_rate"));
	auto rd_countdown = repast::Random::instance()->createUniIntGenerator(1, Grass::growing_rate);

	int id = 0;
	repast::GridDimensions dimensions = grass_grid->dimensions();
	for(int x = dimensions.origin().getX();
			x < dimensions.origin().getX() + dimensions.extents().getX();
			x++) {
		for(int y = dimensions.origin().getY();
				y < dimensions.origin().getY() + dimensions.extents().getY();
				y++) {
			bool grown = repast::Random::instance()->nextDouble() < init_grass_rate;
			int countdown = 0;
			if(!grown)
				countdown = rd_countdown.next();
			repast::AgentId agent_id(id++, rank, GRASS);
			agent_id.currentRank(rank);

			Grass* grass = new Grass(agent_id, grown, countdown);
			grass_context.addAgent(grass);

			repast::Point<int> location(x, y);
			grass_grid->moveTo(grass, location);
		}
	}

}

void PreyPredatorModel::init() {
	init_grass();
	init_preys();
	init_predators();
}

void PreyPredatorModel::initSchedule(repast::ScheduleRunner& runner) {
	runner.scheduleEvent(1.0, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::move
					)
				));
	runner.scheduleEvent(1.1, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::eat
					)
				));
	runner.scheduleEvent(1.2, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::reproduce
					)
				));
	runner.scheduleEvent(1.3, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::die
					)
				));
	runner.scheduleEvent(1.4, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::grow
					)
				));
	runner.scheduleEvent(1.5, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<repast::DataSet>(
					dataset, &repast::DataSet::record
					)
				));
	runner.scheduleEvent(1.6, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<repast::DataSet>(
					dataset, &repast::DataSet::write
					)
				));

	runner.scheduleStop(stop_at);
}

void PreyPredatorModel::move() {
	std::set<PreyPredatorAgent*> local_agents;
	if(agent_context.size()>0) {
		agent_context.selectAgents(
				repast::SharedContext<PreyPredatorAgent>::LOCAL, local_agents
				);
		for(auto agent : local_agents)
			std::cout << "LOCAL AGENT: " << agent->getId() << " - " << agent->getEnergy() << std::endl;

		local_agents.clear();
		agent_context.selectAgents(
				repast::SharedContext<PreyPredatorAgent>::NON_LOCAL, local_agents
				);
		for(auto agent : local_agents)
			std::cout << "DISTANT AGENT: " << agent->getId() << std::endl;

		std::cout << "MOVING AGENTS" << std::endl;

		std::vector<PreyPredatorAgent*> agents;
		agent_context.selectAgents( repast::SharedContext<PreyPredatorAgent>::LOCAL, agents);

		for(auto agent : agents)
			agent->move(*grid);
	}
	
	this->synchronize();
}

void PreyPredatorModel::eat() {
	std::cout << "AGENTS EAT" << std::endl;
	if(agent_context.size() > 0) {
		std::vector<PreyPredatorAgent*> preys;
		agent_context.selectAgents(
				repast::SharedContext<PreyPredatorAgent>::LOCAL, preys
				);

		for(auto agent : preys)
			agent->eat(*grid, *grass_grid);
	}

	this->synchronize();
}

void PreyPredatorModel::reproduce() {
	std::cout << "AGENTS REPRODUCE" << std::endl;

	if(agent_context.size() > 0) {
		std::vector<PreyPredatorAgent*> preys;
		agent_context.selectAgents(
				repast::SharedContext<PreyPredatorAgent>::LOCAL, preys
				);

		for(auto agent : preys)
			agent->reproduce(agent_context, *grid);
	}

	this->synchronize();
}

void PreyPredatorModel::die() {
	std::cout << "AGENTS DIE" << std::endl;
	if(agent_context.size() > 0) {
		std::vector<PreyPredatorAgent*> preys;
		agent_context.selectAgents(
				repast::SharedContext<PreyPredatorAgent>::LOCAL, preys
				);

		for(auto agent : preys)
			agent->die(agent_context, *grid);
	}

	this->synchronize();
}

void PreyPredatorModel::grow() {
	std::vector<Grass*> grass;
	grass_context.selectAgents(repast::SharedContext<Grass>::LOCAL, grass);

	for(auto g : grass)
		g->grow();
}

void PreyPredatorModel::synchronize() {
	std::cout << "SYNC GRID" << std::endl;
	// Marks agents to migrate
	grid->balance();

	/*
	 * Sync preys
	 */
	// Move agents across processes
	repast::RepastProcess::instance()->synchronizeAgentStatus<
		PreyPredatorAgent, PreyPredatorPackage,
		PreyPredatorPackageProvider, PreyPredatorPackageReceiver>(
				agent_context, provider, receiver, receiver);

	// Synchronizes projection: builds agent copies in buffer zones
	repast::RepastProcess::instance()->synchronizeProjectionInfo<
		PreyPredatorAgent, PreyPredatorPackage,
		PreyPredatorPackageProvider, PreyPredatorPackageReceiver>(
				agent_context, provider, receiver, receiver);

	// Updates agent data
	repast::RepastProcess::instance()->synchronizeAgentStates<
		PreyPredatorPackage, PreyPredatorPackageProvider,
		PreyPredatorPackageReceiver>(provider, receiver);
}

PreyPredatorModel::~PreyPredatorModel() {
	delete dataset;
}
