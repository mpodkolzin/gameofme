template<typename StateT>
class Rule {
public:
    virtual ~Rule() = default;
    virtual StateT apply(StateT current, const std::vector<StateT>& neighbors) const = 0;
    virtual std::unique_ptr<Rule<StateT>> clone() const = 0;
};