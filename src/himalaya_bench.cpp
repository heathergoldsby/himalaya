/* himalaya.cpp
 *
 * This file is part of the Himalaya project.
 *
 * Copyright 2012 David B. Knoester.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ea/evolutionary_algorithm.h>
#include <ea/representations/realstring.h>
#include <ea/cmdline_interface.h>
#include <ea/datafiles/fitness.h>
#include <ea/line_of_descent.h>

using namespace ealib;
#include "himalaya.h"

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
#include <ea/meta_population.h>
#include <ea/fitness_functions/benchmarks.h>
#include <ea/generational_models/qhfc.h>


using namespace ealib;

template <typename EA>
struct configuration : public abstract_configuration<EA> {
    //! Called to generate the initial EA population.
    void initial_population(EA& ea) {
        generate_ancestors(ancestors::uniform_real(), get<POPULATION_SIZE>(ea), ea);
    }
    
    void configure(EA& ea) {
        add_event<datafiles::fitness_evaluations>(this, ea);
    }
};

typedef evolutionary_algorithm<
realstring,
mutation::per_site<mutation::uniform_real>,
benchmarks,
configuration, // user-defined configuration methods
recombination::two_point_crossover, // recombination operator
generational_models::steady_state<selection::proportionate< >, selection::elitism<selection::tournament< > > >, // generational model
attr::default_attributes, // individual attributes
individual_lod // using an lod individual automatically turns on LOD tracking.
> ea_type;

/*! Define the EA's command-line interface.  Ealib provides an integrated command-line
 and configuration file parser.  This class specializes that parser for this EA.
 */
template <typename EA>
class cli : public cmdline_interface<EA> {
public:
    //! Define the options that can be parsed.
    virtual void gather_options() {
        add_option<REPRESENTATION_SIZE>(this);
        add_option<POPULATION_SIZE>(this);

        add_option<REPLACEMENT_RATE_P>(this);
        add_option<MUTATION_GENOMIC_P>(this);
        add_option<MUTATION_PER_SITE_P>(this);
        add_option<MUTATION_UNIFORM_REAL_MIN>(this);
        add_option<MUTATION_UNIFORM_REAL_MAX>(this);

        add_option<TOURNAMENT_SELECTION_N>(this);
        add_option<TOURNAMENT_SELECTION_K>(this);
        add_option<ELITISM_N>(this);

        add_option<RUN_UPDATES>(this);
        add_option<RUN_EPOCHS>(this);
        add_option<CHECKPOINT_OFF>(this);
        add_option<CHECKPOINT_PREFIX>(this);
        add_option<RNG_SEED>(this);
        add_option<RECORDING_PERIOD>(this);
        add_option<ANALYSIS_OUTPUT>(this);
        
        add_option<BENCHMARKS_FUNCTION>(this);
    }
    
    //! Define events (e.g., datafiles) here.
    virtual void gather_events(EA& ea) {
        add_event<datafiles::fitness>(this, ea);
        add_event<himalaya>(this,ea); // end of update event that triggers global mutation rate change
        add_event<himalaya_datafile>(this,ea);
//        add_event<himalaya_inheritance>(this,ea); // this is the parent->offspring mutation rate stuff, disabled for now
    };
};

LIBEA_CMDLINE_INSTANCE(ea_type, cli);