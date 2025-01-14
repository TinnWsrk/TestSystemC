#include "KGC.h"
#include <boost/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
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
    boost::random::mt19937 gen(static_cast<unsigned int>(time(0)));
    boost::random::uniform_int_distribution <uint128_t> dist (0,p-1);
    return dist(gen);
}

//methode zur Berechnung sysmmetrischen Polynoms
uint128_t KGC::symmetric_polynomial(uint128_t x, uint128_t y, uint128_t a,uint128_t b, uint128_t c, uint128_t p){
    using boost::multiprecision::pow;
    
    return (a*pow(x,2)+b*pow(y,2)+c*x*y) % p; // Polynom mod p
}


//öffentliche Werte geniereieren, unique Param

void KGC:: generate_unique_public_values(uint128_t p, int num_users){
    std::set<uint128_t> unique_values; //Set zur Überprüng von Unique
    boost::random::mt19937 gen(static_cast<unsigned int>(time(0))); //Zufallszahlengenerator
    
    for(int i=0; i< num_users; ++i){
        uint128_t value;

        //iteriert bis Wert gefunden
        do{
            value = random_element_gf(p); // aus GF(p) zufall Wert gen

        }while (unique_values.find(value) !=unique_values.end()); //prüfen, verdoppeln

        unique_values.insert(value); //eindeutig markieren
        public_values[i]=value; // Wert dem Benutzer mit Index i zuweisen

        //Veranschaulichen
        std::cout << "Benutzer" << i <<": Öffentlicher Wert =" << value << " mod" << p <<std::endl;
    }
}

//Generiertung individueller Polynome
std::map <int, std::function<uint128_t(uint128_t)>> KGC::generate_individual_polynomials(uint128_t a,uint128_t b, uint128_t c,uint128_t p){
    

    
    std::map<int, std::function<uint128_t(uint128_t)>> individual_polynomials;


    //Iteriert über Benutzer und die öff Werte
    for(const auto& [user_id, r_B]: public_values){
        //Lambda-Funktion für das indiv. Polynom
        individual_polynomials[user_id]=[=](uint128_t x){
            return symmetric_polynomial(x,r_B,a,b,c,p);



        };
        //Ausgabe zur Veranschaulichung
        std::cout << " Individuelles Polynom für Benutzer" << user_id<< ":g_B(x) = f(x," << r_B <<")" <<std::endl;
    }
    return individual_polynomials;
}

//Generiert individuell Polynom, und sendet

void KGC::generate_and_send_key(){

    //warte auf Start der Simulation
    wait(SC_ZERO_TIME);

    //Zufallsprimzahl p generieren.
    uint128_t p = generate_random_prime();
    std::cout << "Primzahl p:" << p <<std::endl;


    //Zufällige Koeff für sym Polynom
    uint128_t a = random_element_gf(p);
    uint128_t b = random_element_gf(p);
    uint128_t c = random_element_gf(p);


    generate_unique_public_values(p,num_users);

    //Indivdl Polynom generieren
    auto individual_polynomials = generate_individual_polynomials(a,b,c,p);

    for (const auto& [user_id,r_B] : public_values){
        tlm::tlm_generic_payload trans; // Trans Objekt erstellen
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME; //Verzögerung

        uint128_t x = random_element_gf(p); 
        uint128_t result = individual_polynomials[user_id](x); //Polynom für x berechnen

        //Transaktionsdaten einrichten
        trans.set_command(tlm::TLM_WRITE_COMMAND);
        trans.set_address(user_id);
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(&result)); //Zeiger auf die Daten
        trans.set_data_length(sizeof(result));

        std::cout << "KGC: Send individuell für Benutzer " << user_id << "mit Ergebnis:" << result << std::endl;
        
        //Transaktion senden, über Initator
        iKGCSocket -> b_transport(trans, delay);

        if(trans.get_response_status()!= tlm::TLM_OK_RESPONSE){
            std::cerr << "KGC:Fehler beim Senden der Transaktion an Benutzer" << user_id << std::endl;
        }


    }
}

