#ifndef AFINA_NETWORK_SERVER_H
#define AFINA_NETWORK_SERVER_H

#include <memory>
#include <vector>

namespace Afina {
class Storage;
namespace Network {

/**
 * # Network processors coordinator
 * Configure resources for the network processors and coordinates all work
 */
class Server {
public:
    Server(std::shared_ptr<Afina::Storage> ps) : pStorage(ps) {}
    virtual ~Server() {}

    /**
     * Starts network service. After method returns process should
     * listen on the given interface/port pair to process  incomming
     * data in workers number of threads
     */
    virtual void Start(uint16_t port, uint16_t workers = 5) = 0;

    /**
     * Signal all worker threads that server is going to shutdown. After method returns
     * no more connections should be accept, existing connections should stop receive commands,
     * but must wait until currently run commands executed.
     *
     * After existing connections drain each should be closed and once worker has no more connection
     * its thread should be exit
     */
    virtual void Stop() = 0;

    /**
     * Blocks calling thread until all workers will be stopped and all resources allocated for the network
     * will be released
     */
    virtual void Join() = 0;

protected:
    /**
     * Instance of backing storeage on which current server should execute
     * each command
     */
    std::shared_ptr<Afina::Storage> pStorage;

public:
    // Maximal count of clients to socket in listen() function
    static const int max_listen = 5;
};

} // namespace Network
} // namespace Afina

#endif // AFINA_NETWORK_SERVER_H
