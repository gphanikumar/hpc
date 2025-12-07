
template<typename A>
solver<A>::solver(ryokobayashi1993<A> &kobayshi, const sim_info *s, const Kobayashi_Parameters *kb, IO *io)
    :kobayshi_(kobayshi), s_(s), kb_(kb), io_(io){}



template<typename A>
void solver<A>::time_stepping(){

    for(int i=0; i<s_->time_steps; ++i){
        kobayshi_.evolve();

        if(i % s_->saveT == 0){
            io_->write_output(i, kobayshi_.get_phi(), kobayshi_.get_temp());
            std::cout << "Output written at step " << i << std::endl;
        }
    }

}