#include "KGC.h"
#include <boost/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <map>
#include <set>

using uint128_t = boost::multiprecision::uint128_t;
/*
KGC::KGC(sc_module_name name) : sc_module(name), iKGCSocket("iKGCSocket"){
    SC_HAS_PROCESS(KGC); //register SC_THREAD
    SC_THREAD(generate_and_send_key); //SC thread schlüsselerzeugung
}
*/


//Generiernung p

uint128_t KGC:: generate_random_prime(){
    
    /*
    boost::random::mt19937 gen(static_cast<unsigned int>(time(0)));
    // bereich 10 e 15 bis 10 e 16
    boost:: random::uniform_int_distribution<uint128_t> dist(1e15,1e16);*/


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
    //boost::random::mt19937 gen(static_cast<unsigned int>(time(0)));
    boost::random::uniform_int_distribution <uint128_t> dist (0,p-1);
    return dist(gen);
}

//methode zur Berechnung sysmmetrischen Polynoms
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

}
//öffentliche Werte geniereieren, unique Param

void KGC::generate_unique_public_values(uint128_t p, int num_users){

    std::set<uint128_t> unique_values; //Set zur Überprüng von Unique
    boost::random::mt19937 gen(static_cast<unsigned int>(time(0))); //Zufallszahlengenerator
    
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
std::map <int, std::function<uint128_t(uint128_t)>> KGC::generate_individual_polynomials(uint128_t d,uint128_t p, std::vector<boost::multiprecision::uint128_t>&coefficients){
    

    std::map<int, std::function<uint128_t(uint128_t)>> individual_polynomials;
    
    

    //Iteriert über Benutzer und die öff Werte
    for(const auto& [user_id, r_B]: public_values){
        // berechne g(x) =f(x,r_B)

        std::vector<boost::multiprecision::uint128_t> g_coeff(3); //Koeff für g(x)
        int index =0;
        for (int i=0; i<=d;i++){
            //Berechne die Koeff - r_B ind f(x,y)
            uint128_t coeff =0;
            for(int j=0;j<=d;j++){
                uint128_t term = coefficients[index]*pow(r_B,j); //r_B einsetzen 
                coeff=(coeff+term)%p;
                index ++;

            }
            g_coeff[i] = coeff;
            
        }
        
       //Lamba-Funktion für das individuelle Polynom
        auto g_B_function =[=](uint128_t x){
          uint128_t result=0;
          for(int i=0;i<=d;i++){
             result =(result +g_coeff[i]*pow(x,i))%p;
          }
          return result;

    
        };
       

        //g_B funktion in die Map aufgenommen
        individual_polynomials[user_id]= g_B_function;
        //Debug ausgabe
        std::cout << "Benutzer ID:  " <<user_id<<"g_B(x) =";

        for(int i=0;i<=d;i++){
            std::cout <<g_coeff[i] <<"*x^ "<<i;
            if(i<d) std::cout <<"+";
        }
        std::cout << std::endl;
    
      }
      return individual_polynomials;
}

// Gen sym Coeff
void KGC:: generate_symmetric_coefficients(uint128_t p,std::vector<boost::multiprecision::uint128_t> &coefficients){

    //Index für Koeff
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
    
    std::cout << "Symmetrische Koeffizienten (a_ij) für f(x, y):" << std::endl;
    int index = 0;
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            std::cout << "a_" << i << "," << j << " = " << coefficients[index] << " ";
            index++;
        }
        std::cout << std::endl;
    }
    





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
    std::vector<boost::multiprecision::uint128_t> coefficients(9) ; //Anzahl der Koeffizeinten
   

    generate_symmetric_coefficients(p,coefficients);

    //generate  r_B für jede Benutzer
    generate_unique_public_values(p,num_users);

    //Indivdl Polynom generieren mit a,b,c
    auto individual_polynomials = generate_individual_polynomials(d,p,coefficients);

    //Zähler zur Auswahl des iSocket für KGC
    int socket_selector =0;

    int user_count=0;

    //für jede NA trans erstellen
    for(const auto& [user_id,r_B] : public_values){

        //Wenn alle Benutzer verarbeitet wurden 
        //Bzw. wenn die Anzahl num_user erreicht, dann stop
        

        tlm::tlm_generic_payload trans; // Trans Objekt erstellen
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME; //Verzögerung


        //koeff Vektor für jede Benutzer berechnen
        auto g_B_function = individual_polynomials[user_id];
        
        
        //Array zur Speicherung und Senden von Polynom(result) und r_B(pub_values)
        uint128_t data_array[10];
        int index =0;
        
        //Berechne Koeff von r_B und speicher im data array
        for(int i=0; i<=d;i++){
            for(int j=0;j<=d;j++){

                if(index==10){
                    std::cout <<"Fehler in Index" <<std::endl;
                }
               
                data_array[index] = g_B_function(1);
                
                index++;
            }
        }

        //füg r_B in Array
        data_array[9] =r_B;
        //add p
        data_array[10] =p;

        //Transaktionsdaten einrichten
        trans.set_command(tlm::TLM_WRITE_COMMAND);
        trans.set_address(user_id);
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data_array)); //Zeiger auf die Daten
        trans.set_data_length(sizeof(data_array));

        std::cout << "KGC: Send individuell für Benutzer:   " << user_id << ": : " << data_array[9] <<  std::endl;
        
         std::cout << "Anzahl Benutzer "<< user_count <<std::endl;

        //wähle iSock für KGC na1 bzw. na2

        if(socket_selector==0){
            iKGCSocket1->b_transport(trans,delay); // trans über iSocket 1
            socket_selector=1; //Umschalten
        }else{
            iKGCSocket2->b_transport(trans, delay); //trans über iSocket 2 senden
            socket_selector=0; //umschalten zurück auf 0
        }
       std::cout << "SSSSSS"<< std::endl;
 

        if(trans.get_response_status()!= tlm::TLM_OK_RESPONSE){
            std::cerr << "KGC:Fehler beim Senden der Transaktion an Benutzer" << user_id << std::endl;
        }

        user_count ++; //ein Benutzer abgearbeitet und inkrementiert
        

       


    }
     std::cout << "Alle Benutzer verarbeitet. KGC geht in ruhe Modus :))))"<< std::endl;
     
}

