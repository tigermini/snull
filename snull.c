#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>

struct snull_pkt {
    struct snull_pkt *next;
    struct net_device *net_dev;
    int datalen;
    u8 data[ETH_DATA_LEN];
};

struct snull_priv {
    struct net_device_stats stats;
    int status;
    struct snull_pkt *ppool;
    struct snull_pkt *rx_queue;
    int rx_int_enabled;
    int tx_pktlen;
    u8 *tx_pktdata;
    struct sk_buff *skb;
    spinlock_t lock;
};

struct net_device *snull_netdev[2];

static int snull_init(void)
{

}

void snull_teardown_pool(struct net_device *dev)
{
    struct snull_priv *priv;
    struct snull_pkt *pkt;

    priv = netdev_priv(dev);
    while ((pkt = priv->ppool)) {
        priv->ppool = pkt->next;
        kfree(pkt);
    }
}

void snull_open(){}

void snull_release(){}


void snull_init(struct net_device *dev)
{
    struct snull_priv *priv;

    ether_setup(dev);       /* 对其中一些成员赋值， 在eth.c的366行 */

    dev->open = snull_open;
    dev->stop = snull_release;
}

void snull_cleanup(void)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (snull_netdev[i]) {
            unregister_netdev(snull_netdev[i]);
            snull_teardown_pool(snull_netdev[i]);
            free_netdev(snull_netdev[i]);
        }
    }
}

static int snull_init_module(void)
{
    int i;
    int register_ret;

    snull_netdev[0] = alloc_netdev(sizeof(struct snull_priv), "sn%d", snull_init);
    snull_netdev[1] = alloc_netdev(sizeof(struct snull_priv), "sn%d", snull_init);
    if (snull_netdev[0] == NULL || snull_netdev[1] == NULL) {
        printk("snull_netdev alloc failed!\n");
        return -1;
    }

    for (i = 0; i < 2; i++) {
        if (register_netdev(snull_netdev[i])) {
            printk("snull: register device %s error\n", snull_netdev[i]->name);
            ret = -1;
            goto out;
        }
    }

    return 0;

out:
    if ()
    return ret;
}


module_init(snull_init_module);
module_exit(snull_cleanup);

MODULE_AUTHOR("tiger");
MODULE_LICENSE("GPL");