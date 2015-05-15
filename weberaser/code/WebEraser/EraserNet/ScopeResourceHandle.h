#pragma once

template <class Res, class Deletor>
class ScopeResourceHandle {
	Res res_;
	Deletor deletor_;
	ScopeResourceHandle(const ScopeResourceHandle<Res, Deletor>&) {};
	const ScopeResourceHandle& operator=(const ScopeResourceHandle<Res, Deletor>&) { return *this; }
public:
	ScopeResourceHandle(Res res, Deletor deletor) : res_(res){
		this->deletor_ = deletor;
	}
	~ScopeResourceHandle() {
		this->deletor_(this->res_);
	}
	Res get() {
		return this->res_;
	}
};
