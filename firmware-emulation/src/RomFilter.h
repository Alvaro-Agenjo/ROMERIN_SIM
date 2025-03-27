#pragma once
template<class T,int N, class U=T>
class filter{
T buffer[N]={0};
U acc;
int index=0;
public:
    filter(T init=0){acc=init;for(auto &el:buffer)el=init;}
    T add(T value){
        acc-=buffer[index];
        acc+=(buffer[index]=value);
        index=(index+1)%N;
        return acc/N; 
    }
};