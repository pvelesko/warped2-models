//Implementation of a Forest Fire Simulation

#include <cassert>
#include <random>
#include "forest.hpp"
#include "tclap/ValueArg.h"

WARPED_REGISTER_POLYMORPHIC_SERIALIZABLE_CLASS(ForestEvent)

std::vector<std::shared_ptr<warped::Event> > Forest::initializeLP() {

    // Register random number generator
    //this->registerRNG<std::default_random_engine>(this->rng_);

    std::default_random_engine rng;

    uniform_int_distribution<int> LPS(0, this->width * this->height);

    std::vector<std::shared_ptr<warped::Event> > events;

    events.emplace_back(new ForestEvent {lp_name(LPS(rng), IGNITION, 1}); 


    return events;
}


inline std::string Forest::lp_name(const unsigned int lp_index){

    return std::string("Forest_") + std::to_string(lp_index);
}

<<<<<<< HEAD
=======
std::vector<std::shared_ptr<warped::Event> > Forest::receiveEvent(const warped::Event& event) {

    std::vector<std::shared_ptr<warped::Event> > response_events;
    auto received_event = static_cast<const ForestEvent&>(event);

    switch (received_event.type_) {

        case RADIATION: {
            this->state_.heat_content_=this->state_.heat_content_ + recieved_event.heat_content;
            // if there is enough heat and the vegtation is unburnt Schedule ignition 
            if(this->state_.heat_content_ >= this->ignition_threshold && this->state_burn_status == UNBURNT){
            unsigned int ignition_time = recieved_event.ts+1;
            response_events.emplace_back(new ForestEvent {this->name_, IGNITION, ignition_time });
            }
            break;
        }

        case RADIATION_TIMER: {
            unsigned int radiation_heat=this->state_.heat_content_ /100 * 5
            this->state_.heat_content_ /100 * 95;
            // Schedule Radiation events for each of the eight surrounding LPs
            /*begin for loop*/
            unsigned int radiation_time = received_event.ts_ + 1;
            response_events.emplace_back(new ForestEvent { this->name_, RADIATION,
                                                                            radiation_time });
            /*end for loop*/
            if(this->state_.heat_content_ <= this->burnout_threshold){
            this->state_.burn_status_ = BURNOUT
            }
            else{
            unsigned int radiation_timer = recieved_event.ts + 5;
            response_events.emplace_back(new ForestEvent {this->name_, RADIATION_TIMER, radiation_timer });
            }
            break;
        }
        case IGNITION: {
            this->state_.burn_status_=GROWTH;
            // Schedule Peak Event
            unsigned int peak_time = received_event.ts + ((this->peak_threshold-this->ignition_threshold)/this->heat_rate);
            response_events.emplace_back(new ForestEvent {this->name_, PEAK, peak_time });
            break;
        }
        case PEAK: {
            this->state_.burn_status_=DECAY;
            this->state_.heat_content_=this->state_.heat_content_ + (this->peak_threshold - this->ignition_threshold);
            // Schedule first Radiation Timer
            unsigned int radiation_timer = recieved_event.ts + 5;
            response_events.emplace_back(new ForestEvent {this->name_, RADIATION_TIMER, radiation_timer });
            break;
        }
    }
    return response_events;
}

std::queue<int> ignition_threshold_vector;
std::queue<int> peak_threshold_vector;
unsigned int width,height;
>>>>>>> 36595fdfa98846b2a7d0c8ebd47e7a5f435bb4f4



unsigned char *read_bmp( std::string img_name, unsigned int heat_rate,
                         unsigned int radiation_percent, unsigned int burnout_threshold){

    FILE *fp = fopen(img_name.c_str(), "rb");
    if(!fp) throw "Argument Exception";

    // Read the 54-byte header
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, fp);

    // Extract image height and width from header
    width  = *(unsigned int *)&info[18];
    height = *(unsigned int *)&info[22];

    std::cout << "Width  : " << width    << std::endl;
    std::cout << "Height : " << height   << std::endl;

    unsigned int row_padded = (width*3 + 3) & (~3);
    unsigned char *data = new unsigned char[row_padded];



    for( unsigned int i = 0; i < height; i++ ) {
        fread(data, sizeof(unsigned char), row_padded, fp);
        for(unsigned int j = 0; j < width*3; j += 3) {
            //std::cout   << "B: "<< (int)data[j] 
            //            << " G: " << (int)data[j+1]
            //            << " R: " << (int)data[j+2]
            //            << std::endl;
            unsigned int index_num = i*j; 
            //Placeholder equations for threshold calculation
            unsigned int ignition_threshold = (int)data[j] + (int)data[j+1] + (int)data[j+2];
            unsigned int peak_threshold = ((int)data[j] + (int)data[j+1] + (int)data[j+2]) * 2;
            
            std::string name = Forest::lp_name(index_num)
            lps.emplace_back(name, width, height,ignition_threshold, heat_rate, 
                             peak_threshold, burnout_threshold, index_num);

        }
   }
   fclose(fp);
   return data;
}




int main(int argc, char *argv[],){
   
    std::string config_filename = "map_hawaii.bmp";
    unsigned int heat_rate = 100;
    unsigned int radiation_percent = 5;
    unsigned int burnout_threshold = 50;

    
    TCLAP::ValueArg<std::string> config_arg("m", "map",
                        "Forest model vegetation config", false, config_filename, "string");
    TCLAP::ValueArg<unsigned int> heat_rate_arg("h", "heat-rate", "Speed of growth of the fire",
                                                                false, heat_rate, "unsigned int");
    TCLAP::ValueArg<unsigned int> radition_percent_arg("r", "radiation-percent", 
            "Percent of Heat released every timstamp", false, radiation_percent, "unsigned int");
    TCLAP::ValueArg<unsigned int> burnout_threshold_arg("b", "burnout-threshold",
                                    "Amount of heat needed for a cell to burn out", false, 
                                                            burnout_threshold, "unsigned int");
    std::vector<TCLAP::Arg*> args = {&config_arg, &heat_rate_arg,
                                     &radiation_percent_arg, &burnout_threshold_arg};
   
    config_filename = config_arg.getValue();
    heat_rate = heat_rate_arg.getValue();
    radiation_percent = radiation_percent_arg.getValue();
    burnout_threshold = burnout_threshold.getValue();

    warped::Simulation forest_sim {"Forest Simulation", argc, argv, args};

    std::vector<Forest> lps;
   
    (void) read_bmp(config_filename, heat_rate, radiation_percent, burnout_threshold);

    std::vector<warped::LogicalProcess*> lp_pointers;
    for (auto& lp : lps) {
        lp_pointers.push_back(&lp);
    }

    forest_sim.simulate(lp_pointers);

    

    
    return 0;

}
