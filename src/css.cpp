#ifndef CSS_CPP
#define CSS_CPP

#include "css.hpp"

#include "visitor.hpp"

/**
 * Creates a text value
 * @param value text value
 */
CSS::TextValue::TextValue(std::string value) : value(std::move(value)) {
}

/**
 * Clones a TextValue
 * @return cloned value
 */
CSS::ValuePtr CSS::TextValue::clone() const {
    return CSS::ValuePtr(new CSS::TextValue(*this));
}

/**
 * Prints a text declaration value
 * @return printed value
 */
std::string CSS::TextValue::print() const {
    return value;
}

/**
 * String forms of units
 * @return units as strings
 */
std::vector<std::string> CSS::UnitRaw() {
    return {"px", "em", "rem", "vw", "vh", "%"};
}

/**
 * Creates a unit value
 * @param value magnitude
 * @param unit unit used
 */
CSS::UnitValue::UnitValue(double value, CSS::Unit unit)
    : value(value), unit(unit) {
}

/**
 * Clones a UnitValue
 * @return cloned value
 */
CSS::ValuePtr CSS::UnitValue::clone() const {
    return CSS::ValuePtr(new CSS::UnitValue(*this));
}

/**
 * Prints a unit declaration value
 * @return printed value
 */
std::string CSS::UnitValue::print() const {
    auto     val    = std::to_string(value);
    uint64_t offset = 1;
    if (val.find_last_not_of('0') == val.find('.')) {
        offset = 0;
    }
    val.erase(val.find_last_not_of('0') + offset, std::string::npos);
    return val + CSS::UnitRaw()[unit];
}

/**
 * Creates a color value
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
CSS::ColorValue::ColorValue(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(r), g(g), b(b), a(a) {
}

/**
 * Clones a ColorValue
 * @return cloned value
 */
CSS::ValuePtr CSS::ColorValue::clone() const {
    return CSS::ValuePtr(new CSS::ColorValue(*this));
}

/**
 * Prints a color declaration value
 * @return printed value
 */
std::string CSS::ColorValue::print() const {
    return "rgba(" + std::to_string(r) + ", " + std::to_string(g) + ", " +
           std::to_string(b) + ", " + std::to_string(a) + ")";
}

/**
 * Creates a CSS Selector
 * @param tag selector tag
 * @param id selector id
 * @param klass selector classes
 */
CSS::Selector::Selector(std::string              tag,
                        std::string              id,
                        std::vector<std::string> klass)
    : tag(std::move(tag)), id(std::move(id)), klass(std::move(klass)) {
}

/**
 * Determines the specificity of the selector, prioritized by
 * (id, class, tag). High specificity is more important.
 * @return specificity vector
 */
CSS::Specificity CSS::Selector::specificity() const {
    return {id.empty() ? 0UL : 1, klass.size(), tag.empty() ? 0UL : 1};
}

/**
 * Prints a selector in the form `tag#id.class1.class2`
 * @return pretty-printed selector
 */
std::string CSS::Selector::print() const {
    std::string res(tag);
    if (!id.empty()) {
        res += "#" + id;
    }
    res += std::accumulate(klass.begin(),
                           klass.end(),
                           std::string(),
                           [](auto acc, auto cl) { return acc + "." + cl; });
    return res.empty() ? "*" : res;
}

/**
 * Creates a CSS declaration
 * @param name declaration name
 * @param value declaration value
 */
CSS::Declaration::Declaration(std::string name, CSS::ValuePtr && value)
    : name(std::move(name)), value(std::move(value)) {
}

/**
 * Copy ctor
 * @param rhs declaration to copy
 */
CSS::Declaration::Declaration(const CSS::Declaration & rhs)
    : name(rhs.name), value(rhs.value->clone()) {
}

/**
 * Prints a declaration in the form `name: value;`
 * @return pretty-printed declaration
 */
std::string CSS::Declaration::print() const {
    return name + ": " + value->print() + ";";
}

/**
 * Creates a CSS rule
 * @param selectors selectors to apply rule for
 * @param declarations declarations of rule
 */
CSS::Rule::Rule(PrioritySelectorSet selectors, DeclarationSet declarations)
    : selectors(std::move(selectors)), declarations(std::move(declarations)) {
}

/**
 * Accepts a visitor to the style sheet
 * @param visitor accepted visitor
 */
void CSS::StyleSheet::acceptVisitor(Visitor & visitor) const {
    visitor.visit(*this);
}

#endif