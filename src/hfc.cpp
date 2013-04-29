#include <sstream>
#include <boost/lexical_cast.hpp>
#include <ea/evolutionary_algorithm.h>
#include <ea/fitness_function.h>
#include <ea/selection/proportionate.h>
#include <ea/selection/tournament.h>
#include <ea/selection/elitism.h>
#include <ea/selection/random.h>
#include <ea/generational_models/steady_state.h>
#include <ea/datafiles/fitness.h>
#include <ea/representations/numeric_vector.h>
#include <ea/cmdline_interface.h>
#include <ea/algorithm.h>
#include <ea/adaptive_hfc.h>
#include <ea/meta_population.h>
#include <ea/fitness_functions/benchmarks.h>

using namespace ea;

template <typename EA>
struct configuration : public abstract_configuration<EA> {
    //! Called to generate the initial EA population.
    void initial_population(EA& ea) {
        generate_ancestors(ancestors::uniform_integer(), get<POPULATION_SIZE>(ea), ea);
    }
};


//! Evolutionary algorithm definition.
typedef evolutionary_algorithm<
intstring,
mutation::per_site<mutation::uniform_integer>,
benchmarks,
configuration,
recombination::two_point_crossover, // recombination operator
generational_models::steady_state<selection::proportionate< >, selection::elitism<selection::random> >
> ea_type;


template <typename EA>
struct mp_configuration : public abstract_configuration<EA> {
    //! Called as the final step of EA initialization.
    virtual void initialize(EA& ea) {
        for(std::size_t i=0; i<get<META_POPULATION_SIZE>(ea); ++i) {
        }
    }
};

typedef meta_population<ea_type, mp_configuration> mea_type;


/*! Define the EA's command-line interface.
 */
template <typename EA>
class cli : public cmdline_interface<EA> {
public:
    virtual void gather_options() {
        // ea options
        add_option<META_POPULATION_SIZE>(this);
        add_option<REPRESENTATION_SIZE>(this);
        add_option<INITIALIZATION_PERIOD>(this);
        add_option<EXCHANGE_INDIVIDUALS_PERIOD>(this);
        add_option<ADMISSION_UPDATE_PERIOD>(this);
        add_option<MIN_REMAIN>(this);
        
        add_option<POPULATION_SIZE>(this);
        add_option<REPLACEMENT_RATE_P>(this);
        add_option<ELITISM_N>(this);
        
        add_option<MUTATION_GENOMIC_P>(this);
        add_option<MUTATION_PER_SITE_P>(this);
        add_option<MUTATION_UNIFORM_INT_MIN>(this);
        add_option<MUTATION_UNIFORM_INT_MAX>(this);

        add_option<RUN_UPDATES>(this);
        add_option<RUN_EPOCHS>(this);
        add_option<CHECKPOINT_OFF>(this);
        add_option<CHECKPOINT_PREFIX>(this);
        add_option<RNG_SEED>(this);
        add_option<RECORDING_PERIOD>(this);
        add_option<ANALYSIS_OUTPUT>(this);
        
        add_option<BENCHMARKS_FUNCTION>(this);
        
    
        

    }
    
    virtual void gather_tools() {
    }
    
    virtual void gather_events(EA& ea) {
        add_event<adaptive_hfc>(this, ea);
        add_event<adaptive_hfc_datafile>(this, ea);
        add_event<datafiles::meta_population_fitness>(this, ea);
    };
};
LIBEA_CMDLINE_INSTANCE(mea_type, cli);
