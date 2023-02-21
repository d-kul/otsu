#include <omp.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <exception>
#include <stdexcept>

class input_exception : public std::exception {
public:
    const char* message;
    input_exception() : message() {}
    input_exception(const char* message) : message(message) {}
    const char* what() {
        return message;
    }
};

int main(int argc, char const *argv[])
{
    std::ifstream input;
    std::ofstream output;
    try {
        // setup - precondition check
        if (argc != 4)
            throw input_exception("wrong number of arguments, expected 3");
        input.open(argv[2]);
        output.open(argv[3]);
        if (!input)
            throw input_exception("could not open the input file");
        if (!output)
            throw input_exception("could not open the output file");
        
        int nthreads; bool is_parallel;
        try {
            nthreads = std::stoi(argv[1]);
            is_parallel = (nthreads != -1);
        } catch(std::invalid_argument& ia) { 
            throw input_exception("wrong number of threads");
        } catch(std::out_of_range& ex) { 
            throw input_exception("the number of threads is out of range");
        }
        if (nthreads < -1) 
            throw input_exception("unsupported number of threads");
        
        double time_spent;
        // read file
        int width, height, depth, area;
        std::string header;
        input >> header;
        if (header.compare("P5") != 0)
            throw input_exception("unsupported file format");
        input >> width >> height >> depth;
        area = width*height;
        if (depth != 255)
            throw input_exception("unsupported file format");
        
        unsigned char image[area];
        input.read((char *)image, 1);
        input.read((char *)image, area);
        input.close();

        int maxthreads = omp_get_max_threads();
        nthreads = (nthreads > maxthreads ? maxthreads : nthreads);
        if (nthreads == -1) omp_set_num_threads(1);
        else if (nthreads > 0) omp_set_num_threads(nthreads);

        int i_best = 0;
        unsigned int f0_best = 0;
        double disp_b_max = 0.;
        unsigned int f0 = 0;

        time_spent = omp_get_wtime();
        // get histogram and dependent values
        int hist[256] = {};
        double freq[256], freq_pref[257] = {}, mean = 0.;
        double weighted_pref[257] = {};

        #pragma omp parallel if (is_parallel)
        {
            #pragma omp single
                if (nthreads <= 0) nthreads = omp_get_num_threads();
            int hist_local[256] = {};
            
            #pragma omp for
            for (int i = 0; i < area; i++) {
                hist_local[(int)(unsigned char)image[i]]++;
            }
            for (int j = 0; j < 256; j++) {
                #pragma omp atomic
                hist[j] += hist_local[j];
            }
            #pragma omp barrier

            #pragma omp single
            {
            for (int f = 0; f < 256; f++) {
                freq[f] = hist[f]/(double)area;
                weighted_pref[f+1] = weighted_pref[f] + f*freq[f];
                freq_pref[f+1] = freq_pref[f] + freq[f];
            }
            mean = weighted_pref[256];
            }
            
            #pragma omp barrier

            #pragma omp for private(f0)
            for (int i = 0; i < 255; i++) {
                f0 = i;
                // count values
                double q1 = freq_pref[f0+1];
                double q2 = 1. - freq_pref[f0+1];
                double mean1 = weighted_pref[f0+1]/q1;
                double mean2 = (mean - weighted_pref[f0+1])/q2;
                double disp_b = q1*mean1*mean1 + q2*mean2*mean2;

                #pragma omp critical
                {
                    #pragma omp flush (disp_b,i_best)
                    if (disp_b > disp_b_max || (disp_b == disp_b_max && i > i_best)) {
                        disp_b_max = disp_b;
                        i_best = i;
                    }
                    #pragma omp flush (disp_b,i_best)
                }
            }
            
            #pragma omp barrier

            #pragma omp single
            {    
                f0_best = i_best;
                std::cout << f0_best << ' ' << f0_best << ' ' << f0_best << '\n';
            }

            #pragma omp barrier

            #pragma omp for
            for (int i = 0; i < area; i++) {
                image[i] = (image[i] <= f0_best ? (unsigned char)0 : (unsigned char)255);
            }
        }
        time_spent = omp_get_wtime() - time_spent;
        
        output << "P5\n" 
        << width << ' ' << height << '\n'
        << depth << '\n';
        output.write((char *)image, area);
        output.close();

        // end of the program
        std::cout << "Time (" << nthreads << " thread(s)): " << time_spent * 1000. << " ms\n";
        
        //std::cout << f0_best << ',' << f1_best << ',' << f2_best << ',' << int(time_spent * 1000 + 0.5) << '\n';
    } catch (input_exception& ie) {
        std::cerr << ie.what() << '\n';
        if (input.is_open()) input.close();
        if (output.is_open()) output.close();
        return 1;
    }
    return 0;
}
