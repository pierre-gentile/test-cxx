//
//  Hello World server
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>

using std::string;


int main (void)
{
	try {
		zmq::context_t context(1);
	
		zmq::socket_t socket1(context, ZMQ_REP);
		socket1.bind("tcp://*:9000");

		while (true) {
			std::cout << "Waiting..." << std::endl;
		
			//  Wait for next request from client
			zmq::message_t request;
			socket1.recv(&request);
			
			string const requestData((char const *) request.data(), request.size());
			std::cout << "Received: " << requestData << std::endl;

			//  Do some 'work'
			sleep(1);

			// Send reply back to client
			string const data("World");
			zmq::message_t reply(data.size() * sizeof(string::value_type));
			std::memcpy(reply.data(), data.c_str(), reply.size());
			
			socket1.send(reply);
		}
	}
	catch (zmq::error_t const & err) {
		std::cout << "Erreur : " << err.what() << std::endl;
	}

	return 0;
}
