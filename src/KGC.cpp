#include "KGC.h"
#include <boost/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <map>
#include <set>
#include<chrono>

using uint128_t = boost::multiprecision::uint128_t;
/*
KGC::KGC(sc_module_name name) : sc_module(name), iKGCSocket("iKGCSocket"){
    SC_HAS_PROCESS(KGC); //register SC_THREAD
    SC_THREAD(generate_and_send_key); //SC thread schlüsselerzeugung
}
*/


//Generiernung p

uint128_t KGC:: generate_random_prime(){
    
    //return 7; 
    
    //boost::random::mt19937 gen(static_cast<unsigned int>(time(0)));
    // bereich 10 e 15 bis 10 e 16
    //boost:: random::uniform_int_distribution<uint128_t> dist(1e15,1e16);


    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist64(0,std::numeric_limits<uint64_t>::max());

    
    //Bereich 10e15-10e16
    uint128_t min= static_cast<uint128_t>(1e15);
    uint128_t max = static_cast<uint128_t>(1e16);

    while(true){
        uint64_t high = dist64(gen); // obere 64 bit
        
        uint64_t low = dist64(gen);


        uint128_t candidate = (static_cast<uint128_t>(high)<<64) | low;

        candidate = min + (candidate % (max-min+1)); // begrenzt auf bereich min- max

        candidate |= 1; //ungerade Zahl

        if(miller_rabin_test(candidate,25)){ 
            return candidate; //p gefunden, return
        }
    }
    
}


void KGC::set_num_user(int num_users)
{
    this ->num_users=num_users;
}



//generiert Zufallszahlen aus GF(p)
uint128_t KGC::random_element_gf(uint128_t p){
    unsigned int seed =static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count());
    boost::random::mt19937 gen(seed);
    //boost::random::uniform_int_distribution <uint128_t> dist (0,p-1);
    boost::random::uniform_int_distribution<uint128_t>dist(2,p-1);
    return dist(gen);
}

//methode zur Berechnung sysmmetrischen Polynoms
/*
uint128_t KGC::symmetric_polynomial(uint128_t x, uint128_t y, uint128_t d,uint128_t coefficients[], uint128_t p){
    using boost::multiprecision::pow;

    //result von dem Polynom
    uint128_t result =0;

    //Berechnung des Polynoms mit dynamischem Grad d

    //index in Coeff
    int index =0;
    for(int i=0;i<=d;i++){
        for(int j =0;j<=d;j++){
            uint128_t term = coefficients[index]*pow(x,i)*pow(y,j);
            result=(result +term)%p; //add und mod
            index ++;
        }
    }
    
    return result;

}*/
//öffentliche Werte geniereieren, unique Param

void KGC::generate_unique_public_values(uint128_t p, int num_users){

    std::set<uint128_t> unique_values; //Set zur Überprüng von Unique
    unsigned int seed =static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count());
    boost::random::mt19937 gen(seed); //Zufallszahlengenerator
    
    for(int i=0; i< num_users; ++i){
        uint128_t value;

        //iteriert bis Wert gefunden
        do{
            value = random_element_gf(p); // aus GF(p) zufall Wert gen

        }while (unique_values.find(value) !=unique_values.end()); //prüfen, verdoppeln

        unique_values.insert(value); //eindeutig markieren
        public_values.insert({i,value}); // Wert dem Benutzer mit Index i zuweisen

        //Veranschaulichen
        std::cout << "Benutzer" << i <<": Öffentlicher Wert =" << value << " mod" << p <<std::endl;
    }
}

//Generiertung individueller Polynome
std::map <int, std::vector<boost::multiprecision::uint128_t>> KGC::generate_individual_polynomials(uint128_t d,uint128_t p, std::vector<boost::multiprecision::uint128_t>&coefficients){
    

    std::map<int, std::vector<uint128_t>> individual_polynomials;
    
    

    //Iteriert über Benutzer und die öff Werte
    for(const auto& [user_id, r_B]: public_values){
        

        //berechnen coefficients
        
        boost::multiprecision::cpp_int term = ((coefficients[0]+ coefficients[1]*r_B) % p);
        boost::multiprecision::cpp_int term2= ((coefficients[1]+ coefficients[2]*r_B) % p);

        uint128_t coef = static_cast<uint128_t>(term);
        uint128_t coef2 = static_cast<uint128_t>(term2);       

         // berechne g(x) =f(x,r_B)
        uint128_t max_uint128 = std::numeric_limits<uint128_t>::max();
        std::vector<boost::multiprecision::uint128_t> g_coeff(2); //Koeff für g(x)

            g_coeff[0]=coef;
            g_coeff[1]=coef2;
          
           
            
            //g_coeff[i] = coeff;
            
        
        std::cout<< "Coef sind jetzt:";
        for (size_t i = 0; i < g_coeff.size(); i++)   {
            std::cout <<g_coeff[i];
            if(i!=g_coeff.size()-1){
                std::cout<<", ";
            }
        }
        individual_polynomials[user_id] = g_coeff;

        /*
    for (const auto& entry : individual_polynomials) {
    std::cout << "  Benutzer ID: " << entry.first << " -> g_coeff: ";
    for (const auto& coeff : entry.second) {
        std::cout << coeff << " ";  // Ausgabe jedes Koeffizienten im Vektor
    }
    std::cout << std::endl;
    }
        */
        

        
       
        

        
    
      

      //std::cout<<"Hier wird die Koeffizienten die Benutzer berechnet =="<< individual_polynomials<< std::endl;
      
    }
    return individual_polynomials;

}

// Gen sym Coeff
void KGC:: generate_symmetric_coefficients(uint128_t p,std::vector<boost::multiprecision::uint128_t> &coefficients){
 
    uint128_t num; //Kofficient Element für Vektor

    for (int i = 0; i < 3; i++)
    {

       coefficients[i] = random_element_gf(p);


    }
    

    //Index für Koeff
    /*
    uint128_t matrix [3][3];

    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j<3; j++)
        {
            uint128_t num = random_element_gf(p);
            matrix[i][j] =num;
            matrix[j][i] = num;
        }
        
    }

    int id=0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            coefficients[id++] = matrix[i][j];
        }
        
    }
    */
    
   

}
//Generiert individuell Polynom, und sendet

void KGC::generate_and_send_key(){

     std::cout << "Start Key Generation and Send key"<< std::endl;
    //warte auf Start der Simulation
    wait(SC_ZERO_TIME);
    

    //Zufallsprimzahl p generieren.
    uint128_t p = generate_random_prime();
    std::cout << "Primzahl p:" << p <<std::endl;


    //Zufällige Koeff für sym Polynom
    std::vector<boost::multiprecision::uint128_t> coefficients(3) ; //Anzahl der Koeffizeinten
   

    generate_symmetric_coefficients(p,coefficients);
    std::cout << "Coefficients: ";
    for (const auto& value : coefficients) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    //generate  r_B für jede Benutzer
    generate_unique_public_values(p,num_users);

    //Indivdl Polynom generieren mit a,b,c
    auto individual_polynomials = generate_individual_polynomials(d,p,coefficients);

    //Zähler zur Auswahl des iSocket für KGC
    int socket_selector =0;

    int user_count=0;

    //für jede NA trans erstellen
    for(const auto& [user_id,r_B] : public_values){

        
        

        tlm::tlm_generic_payload trans; // Trans Objekt erstellen
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME; //Verzögerung


        //koeff Vektor für jede Benutzer berechnen
        auto g_coeff_vector = individual_polynomials[user_id];
        
        
        //Array zur Speicherung und Senden von Polynom(result) und r_B(pub_values)
        //std::vector<uint128_t>data_array;
        std::array<boost::multiprecision::uint128_t,4> data_array;
        
        
        //Berechne Koeff von r_B und speicher im data array
        for (int i =0;i<2;++i){
            data_array[i]=g_coeff_vector[i];
        }

        //füg r_B in Array
        data_array[2]=r_B;
        //add p
        data_array[3]=p;

        //Transaktionsdaten einrichten
        trans.set_command(tlm::TLM_WRITE_COMMAND);
        trans.set_address(user_id);
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data_array)); //Zeiger auf die Daten
        trans.set_data_length(sizeof(data_array));

        std::cout << "KGC: Send individuell für Benutzer:   " << user_id << ": : " << data_array[3] <<  std::endl;
        
        std::cout << "Anzahl Benutzer "<< user_count <<std::endl;

        //wähle iSock für KGC na1 bzw. na2

        if(socket_selector==0){
            iKGCSocket1->b_transport(trans,delay); // trans über iSocket 1
            socket_selector=1; //Umschalten
        }else{
            iKGCSocket2->b_transport(trans, delay); //trans über iSocket 2 senden
            socket_selector=0; //umschalten zurück auf 0
        }
       
 

        if(trans.get_response_status()!= tlm::TLM_OK_RESPONSE){
            std::cerr << "KGC:Fehler beim Senden der Transaktion an Benutzer" << user_id << std::endl;
        }

        user_count ++; //ein Benutzer abgearbeitet und inkrementiert
        

       


    }
     std::cout << "Alle Benutzer verarbeitet. KGC geht in ruhe Modus :))))"<< std::endl;
     
}

