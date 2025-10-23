#include <iostream>


class SmokerCtrller; // declare SmokerCtrller class so that SmokerUI class can do stuff before actually defining the class

struct SmokeOp {
    public:
        int Tm{}; // measured temperature
        int Tt{}; // target temperature
        int start{};
        int duration{};
        int elapsed{};
        bool spritzEnabled{};
        int timeToNextSpritz{};
        double fuelLevel{};
};

class SmokerUI {
    protected:
        SmokerCtrller *p_sctrl; // pointer to smoker controller object to set target temperature

    public:
        SmokeOp smokeOpCopy;

        SmokerUI() : smokeOpCopy{}, p_sctrl(nullptr) {};

        void attachSmokerCtrller(SmokerCtrller* _tc) {
            p_sctrl = _tc;
        }

        SmokeOp getSmokeOp() {
            return smokeOpCopy;
        }

        // Basically just a convenient setter function that's meant to be called specifically by SmokerCtrllr
        void setSmokeOp(SmokeOp newSmokeOp) {
            smokeOpCopy = newSmokeOp; // assign by value (not reference) on purpose. Slightly inefficient memory usage but wtv
        }

        // setSmokeOps the SmokerCtrller with any parameter changes from this UI
        void sendSmokeOp(); // declare function, only define after smoker controller definition
};

class CtrlPanel : public SmokerUI {
    public:
        void setSmokeOp(SmokeOp newSmokeOp) {
            SmokerUI::setSmokeOp(newSmokeOp);
            // print some ctrlpanel specific stuff if u want
        }
};

class Website : public SmokerUI {
    public:
        void setSmokeOp(SmokeOp newSmokeOp) {
            SmokerUI::setSmokeOp(newSmokeOp);
            // print some website specific stuff if u want
        }
};

class SmokerCtrller {
    private:
        CtrlPanel* pcp;
        Website* pws;
    public:
        SmokeOp trueSmokeOp {};

        SmokeOp getSmokeOp() {
            return trueSmokeOp;
        }

        void setSmokeOp(SmokeOp newSmokeOp) {
            trueSmokeOp = newSmokeOp;
        }

        void attachCtrlPanel(CtrlPanel* _pcp) {
            pcp = _pcp;
        }

        void attachWebsite(Website* _pws) {
            pws = _pws;
        }

        SmokerCtrller(CtrlPanel& _cp, Website& _ws) : pcp(&_cp), pws(&_ws), trueSmokeOp{} {
            notify();
            _cp.attachSmokerCtrller(this);
            _ws.attachSmokerCtrller(this);
        }

        void notify() {    
            pcp->setSmokeOp(trueSmokeOp);
            pws->setSmokeOp(trueSmokeOp);
        }
};

void SmokerUI::sendSmokeOp() {
    p_sctrl->setSmokeOp(smokeOpCopy);
}

int main()
{
    CtrlPanel myCP = CtrlPanel();
    Website myWS = Website();
    SmokerCtrller sctrl(myCP, myWS);

    myCP.smokeOpCopy.Tt = 100;
    myCP.smokeOpCopy.spritzEnabled = true;
    myCP.sendSmokeOp();
    sctrl.notify();

    myWS.smokeOpCopy.Tt = 110;
    myWS.sendSmokeOp();
    sctrl.notify();

    std::cout << sctrl.getSmokeOp().duration << std::endl;
}
