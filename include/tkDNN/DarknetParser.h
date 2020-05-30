#pragma once
#include <iostream>
#include "tkdnn.h"

namespace tk { namespace dnn {

    struct darknetFields_t{
        std::string type = "";
        int width = 0;
        int height = 0;
        int channels = 3;
        int batch_normalize=0;
        int groups = 1;
        int filters=1;
        int size_x=1;
        int size_y=1;
        int stride_x=1;
        int stride_y=1;
        int padding_x = 0;
        int padding_y = 0;
        int n_mask = 0;
        int classes = 20;
        int num = 1;
        int pad = 0;
        float scale_xy = 1;
        std::vector<int> layers;
        std::string activation = "linear";

    };

    std::ostream& operator<<(std::ostream& os, const darknetFields_t& f){
        os << f.width << " " << f.height << " " << f.channels << " " << f.batch_normalize<< " " << f.filters << " "  << f.activation<< " " << f.scale_xy;
        return os;
    }

    std::string darknetParseType(const std::string& line){
            size_t start = line.find("[");
            size_t end  = line.find("]");
            if( start == std::string::npos || end == std::string::npos)
                return "";
            start++;
            std::string type = line.substr(start, end-start);
            return type;
    }

    bool divideNameAndValue(const std::string& line, std::string&name, std::string& value){
        size_t sep = line.find("=");
        if(sep == std::string::npos)
            return false;
        
        name   = line.substr(0, sep);   
        value  = line.substr(sep+1, line.size() - (sep+1));   
        return true;
    }

    std::vector<int> fromStringToIntVec(const std::string& line, const char delimiter){
        std::stringstream linestream(line);
        std::string value;
        std::vector<int> values;

        while(getline(linestream,value,delimiter))
            values.push_back(std::stoi(value));
        return values;
    }

    bool darknetParseFields(const std::string& line, darknetFields_t& fields){

        std::string name,value;
        if(!divideNameAndValue(line, name, value))
            return false;
        if(name.find("width") !=  std::string::npos)
            fields.width = std::stoi(value);
        else if(name.find("height") !=  std::string::npos)
            fields.height = std::stoi(value);
        else if(name.find("channels") !=  std::string::npos)
            fields.channels = std::stoi(value);
        else if(name.find("batch_normalize") !=  std::string::npos)
            fields.batch_normalize = std::stoi(value);
        else if(name.find("filters") !=  std::string::npos)
            fields.filters = std::stoi(value);
        else if(name.find("activation") !=  std::string::npos)
            fields.activation = value;
        else if(name.find("size") !=  std::string::npos){
            fields.size_x = std::stoi(value);
            fields.size_y = std::stoi(value);
        }
        else if(name.find("size_x") !=  std::string::npos)
            fields.size_x = std::stoi(value);
        else if(name.find("size_y") !=  std::string::npos)
            fields.size_y = std::stoi(value);
        else if(name.find("stride") !=  std::string::npos){
            fields.stride_x = std::stoi(value);
            fields.stride_y = std::stoi(value);
        }
        else if(name.find("stride_x") !=  std::string::npos)
            fields.stride_x = std::stoi(value);
        else if(name.find("stride_y") !=  std::string::npos)
            fields.stride_y = std::stoi(value);
        else if(name.find("pad") !=  std::string::npos)
            fields.pad = std::stoi(value);
        else if(name.find("classes") !=  std::string::npos)
            fields.classes = std::stoi(value);
        else if(name.find("num") !=  std::string::npos)
            fields.num = std::stoi(value);
        else if(name.find("groups") !=  std::string::npos)
            fields.groups = std::stoi(value);
        else if(name.find("scale_xy") !=  std::string::npos)
            fields.scale_xy = std::stof(value);
        else if(name.find("from") !=  std::string::npos)
            fields.layers.push_back(std::stof(value));
        else if(name.find("mask") !=  std::string::npos){
            auto vec = fromStringToIntVec(value, ',');
            fields.n_mask = vec.size();
        }
        else if(name.find("layers") !=  std::string::npos)
            fields.layers = fromStringToIntVec(value, ',');

        else
            std::cout<<"Not supported field: "<<line<<std::endl;
        return true;
    }

    tk::dnn::Network *darknetAddNet(darknetFields_t &fields) {
        std::cout<<"Add Net: "<<fields.type<<"\n";
        dataDim_t dim(1, fields.channels, fields.height, fields.width);
        return new tk::dnn::Network(dim);
    }


    void darknetAddLayer(tk::dnn::Network *net, darknetFields_t &f, std::string wgs_path, std::vector<tk::dnn::Layer*> &netLayers) {
        if(net == nullptr)
            FatalError("Cant add a layer without a Net\n");

        // padding compute
        if(f.pad == 1) {
            f.padding_x = f.padding_y = f.size_x /2;
        }

        std::cout<<"Add layer: "<<f.type<<"\n";
        if(f.type == "convolutional") {
            std::string wgs = wgs_path + "/c" + std::to_string(netLayers.size()) + ".bin";
            printf("%d (%d,%d) (%d,%d) (%d,%d) %s %d %d\n", f.filters, f.size_x, f.size_y, f.stride_x, f.stride_y, f.padding_x, f.padding_y, wgs.c_str(), f.batch_normalize, f.groups);
            netLayers.push_back(new tk::dnn::Conv2d(net, f.filters, f.size_x, f.size_y, f.stride_x, 
                                f.stride_y, f.padding_x, f.padding_y, wgs, f.batch_normalize, false, f.groups));
            if(f.activation != "linear") {
                tkdnnActivationMode_t act;
                if(f.activation == "relu") act = tkdnnActivationMode_t(CUDNN_ACTIVATION_RELU);
                else if(f.activation == "leaky") act = tk::dnn::ACTIVATION_LEAKY;
                else if(f.activation == "mish") act = tk::dnn::ACTIVATION_MISH;
                else { FatalError("activation not supported: " + f.activation); }
                new tk::dnn::Activation(net, act);
            }
        } else if(f.type == "shortcut") {
            if(f.layers.size() != 1) FatalError("no layers to shortcut\n");
            int layerIdx = f.layers[0];
            if(layerIdx < 0)
                layerIdx = netLayers.size() + layerIdx; 
            if(layerIdx < 0 || layerIdx >= netLayers.size()) FatalError("impossible to shortcut\n");
            std::cout<<"shortcut to "<<layerIdx<<" "<<netLayers[layerIdx]->getLayerName()<<"\n";
            netLayers.push_back(new tk::dnn::Shortcut(net, netLayers[layerIdx]));

        } else if(f.type == "upsample") {
            netLayers.push_back(new tk::dnn::Upsample(net, f.stride_x));

        } else if(f.type == "route") {
            if(f.layers.size() == 0) FatalError("no layers to Route\n");
            std::vector<tk::dnn::Layer*> layers;
            for(int i=0; i<f.layers.size(); i++) {
                int layerIdx = f.layers[i];
                if(layerIdx < 0)
                    layerIdx = netLayers.size() + layerIdx; 
                if(layerIdx < 0 || layerIdx >= netLayers.size()) FatalError("impossible to route\n");
                std::cout<<"Route to "<<layerIdx<<" "<<netLayers[layerIdx]->getLayerName()<<"\n";
                layers.push_back(netLayers[layerIdx]);
            }
            netLayers.push_back(new tk::dnn::Route(net, layers.data(), layers.size()));

        } else if(f.type == "yolo") {
            std::string wgs = wgs_path + "/g" + std::to_string(netLayers.size()) + ".bin";
            printf("%d %d %s %d %f\n", f.classes, f.num/f.n_mask, wgs.c_str(), f.n_mask, f.scale_xy);
            netLayers.push_back(new tk::dnn::Yolo(net, f.classes, f.num/f.n_mask, wgs, f.n_mask, f.scale_xy));

        } else{
            FatalError("layer not supported: " + f.type);
        }
    }

    tk::dnn::Network* darknetParser(std::string cfg_file, std::string wgs_path) {

        tk::dnn::Network *net = nullptr;
        
        // layers without activations to retrive correct id number
        std::vector<tk::dnn::Layer*> netLayers;

        std::ifstream if_cfg(cfg_file);
        if(!if_cfg.is_open())
            FatalError("cloud not open cfg file: " + cfg_file);

        darknetFields_t fields; // will be filled with layers fields
        std::string line;
        while(std::getline(if_cfg, line)) {
            // remove comments
            std::size_t found = line.find("#");
            if ( found != std::string::npos ) {
                line = line.substr(0, found);
            }

            // skip empty lines
            if(line.size() == 0)
                continue;
            
            std::string type = darknetParseType(line);
            if(type.size() > 0) {
                // end of filled type
                if(fields.type != "") {
                    if(fields.type == "net")
                        net = darknetAddNet(fields);
                    else
                        darknetAddLayer(net, fields, wgs_path, netLayers);
                }

                // new type
                //std::cout<<"type: "<<type<<"\n";
                fields = darknetFields_t();
                fields.type = type;
                continue;
            }

            if(darknetParseFields(line, fields)) {
                // already parsed do nothing
            } else {
                FatalError("could not parse line: " + line);
            }
        }

        // end of filled type
        if(fields.type != "") {
            darknetAddLayer(net, fields, wgs_path, netLayers);
        }

        if(net == nullptr) {
            FatalError("net not found\n");
        }
        return net;
    }
        
    

}}
