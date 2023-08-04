#include "curve.hpp"
#include "curve_db.hpp"

#include <cmath>
#include <regex>
#include <iterator>

Curve::Segs::Segs(Segs::args args)
    : speed    {args.speed * -1.0},
      startval {args.startval},
      endval   {args.endval}
{}

std::size_t scale_fpos(double fpos)
{
    if (fpos < 0.0) { fpos = 0.0; }
    if (fpos > 1.0) { fpos = 1.0; }

    return static_cast<std::size_t>(std::round(fpos * (Curve::tab_lenf)));
}

void Curve::Alg::process(Curve& c, double fendpos, double fstartpos)
{
    if (fstartpos == auto_startpos) {
        fstartpos = c.last_endpos();
    }

    std::size_t endpos   = scale_fpos(fendpos);
    std::size_t startpos = scale_fpos(fstartpos);

    if (endpos < startpos) {
        auto endmemo = endpos;
        endpos = startpos;
        startpos = endmemo;
    }

    auto dist = static_cast<double>(endpos - startpos);

    for (std::size_t i = startpos; i < endpos; ++i) {
        c.table[i] = this->inner_process(c, (i - startpos)/dist);
    }
}

const Curve::entry_t Curve::Segs::inner_process(Curve& c, Curve::seek_t pos)
{
    if (speed == 0.0) {
            return startval + pos*(endval - startval);
    } else {
        double numer = (endval - startval) * (1.0 - std::exp(pos*speed));
        double denom = 1.0 - std::exp(speed);
        auto out = startval + numer / denom;

        return out;
    }
}

Curve::Soid::Soid(Soid::args args)
    : harmon   {args.harmon},
      phase    {args.phase},
      strength {args.strength},
      offset   {args.offset}
{}

const Curve::entry_t Curve::Soid::inner_process(Curve& c, Curve::seek_t pos)
{
    return strength * std::sin(2*M_PI * harmon * (pos + phase)) + offset;
}

Curve::Soidser::Soidser(Soidser::args args)
    : part_spacing  {args.part_spacing},
      coef_spacing  {args.coef_spacing},
      phase_spacing {args.phase_spacing},
      fund          {args.fund}
{
    if (coef_spacing == same_as_part) {
        coef_spacing = part_spacing;
    }
}

// TODO: get the actual sample rate
static constexpr double sample_rate = 48000;

const Curve::entry_t Curve::Soidser::inner_process(Curve& c, Curve::seek_t pos)
{
    double out = 0.0;

    double part         = 1.0;
    double inv_amp_coef = 1.0;
    double phase        = 0.0;
    while (part*fund < sample_rate/2.0) {
        out += std::sin(2*M_PI * part * (1.0 + phase + pos)) / inv_amp_coef;

        part += part_spacing;

        if (coef_spacing == nsqrd_coefs) {
            inv_amp_coef = std::pow(part, 2.0);
        } else {
            inv_amp_coef += coef_spacing;
        }

        phase += phase_spacing;
    }

    return out;
}

Curve::Curve(std::shared_ptr<CurveDB> ndb)
    : db {ndb}
{}

Curve::Curve(std::shared_ptr<CurveDB> ndb, std::string nname)
    : db {ndb},
      name {nname}
{}

Curve::Curve(std::shared_ptr<CurveDB> ndb, DB::id_t nid)
    : db {ndb},
      id {nid}
{}

Curve::Curve(std::shared_ptr<CurveDB> ndb, DB::id_t nid, std::string nname)
    : db {ndb},
      id {nid},
      name {nname}
{}

enum class lex {
    segs, soid, soidser, nsqrd, num, no_match
};

typedef std::tuple<std::string, lex> lex_t;
typedef std::tuple<std::regex, lex>  lex_reg_t;

struct parse_params {
    double endpos = 1.0;
    double startpos = 0.0;
    unsigned algpos = 0;
    bool provide_startpos = false;
};

template<typename T>
void parse_curveargs(Curve& c,
                     const std::vector<lex_t>& lexes,
                     const struct parse_params& prms,
                     const std::vector<std::function<void(typename T::args&,
                                                          unsigned,
                                                          lex,
                                                          std::string)>>&
                         argfns)
{
    typename T::args args {};

    auto max_args = argfns.size();

    for (unsigned offs = 0; offs < max_args; ++offs) {
        auto argpos = prms.algpos + offs + 1;
        if (lexes.size() > argpos) {
            lex         lme = std::get<1>(lexes.at(argpos));
            std::string tok = std::get<0>(lexes.at(argpos));

            argfns[offs](args, argpos, lme, tok);
        }
    }

    T alg {args};
    if (prms.provide_startpos) {
        alg.process(c, prms.endpos, prms.startpos);
    } else {
        alg.process(c, prms.endpos);
    }
}

Curve& Curve::parse(std::string stmt)
{
    static const std::vector<lex_t> lexeme_strs = {
        {R"(segs)", lex::segs},
        {R"(soid)", lex::soid},
        {R"(soidser)", lex::soidser},
        {R"(n)", lex::nsqrd},
        {R"(-{0,1}[\d]*\.{0,1}[\d]*)", lex::num},
    };

    static const std::vector<lex_reg_t> lexemes = []
    {
        std::vector<lex_reg_t> rs(lexeme_strs.size());
        for (auto&& s : lexeme_strs) {
            std::regex r {std::get<0>(s)};
            lex_reg_t t = {r, std::get<1>(s)};
            rs.push_back(t);
        }
        return rs;
    }();

    std::istringstream strm {stmt};
    typedef std::istream_iterator<std::string> iter_t;

    std::vector<lex_t> lexes;

    for (iter_t iter {strm}; iter != iter_t(); ++iter) {
        lex res = lex::no_match;
        for (auto&& l : lexemes) {
            if (std::regex_match(*iter, std::get<0>(l))) {
                res = std::get<1>(l);
                break;
            }
        }

        if (res == lex::no_match) {
            throw std::runtime_error("did not understand '" + *iter + "'");
        } else {
            lex_t l {*iter, res};
            lexes.push_back(l);
        }
    }

    struct parse_params prms {};

    if (lexes.size() > 1 && std::get<1>(lexes.at(0)) == lex::num) {
        prms.endpos = std::stod(std::get<0>(lexes.at(0)));
        ++prms.algpos;

        if (lexes.size() > 2 && std::get<1>(lexes.at(1)) == lex::num) {
            prms.startpos = std::stod(std::get<0>(lexes.at(1)));
            ++prms.algpos;
            prms.provide_startpos = true;
        }
    }

    lex alg = std::get<1>(lexes.at(prms.algpos));

    if (alg == lex::segs) {
        parse_curveargs<Segs>(*this, lexes, prms, {
            [](Segs::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.speed = std::stod(tok);
            },

            [](Segs::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.startval = std::stod(tok);
            },

            [](Segs::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.endval = std::stod(tok);
            },
        });
    } else if (alg == lex::soid) {
        parse_curveargs<Soid>(*this, lexes, prms, {
            [](Soid::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.harmon = std::stod(tok);
            },

            [](Soid::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.phase = std::stod(tok);
            },

            [](Soid::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.strength = std::stod(tok);
            },

            [](Soid::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.offset = std::stod(tok);
            },
        });
    } else if (alg == lex::soidser) {
        parse_curveargs<Soidser>(*this, lexes, prms, {
            [](Soidser::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.part_spacing = std::stod(tok);
            },

            [](Soidser::args& args, unsigned argpos, lex lme, std::string tok)
            {
                if (lme == lex::nsqrd) {
                    args.coef_spacing = Soidser::nsqrd_coefs;
                } else {
                    args.coef_spacing = std::stod(tok);
                }
            },

            [](Soidser::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.phase_spacing = std::stod(tok);
            },

            [](Soidser::args& args, unsigned argpos, lex lme, std::string tok)
            {
                args.fund = std::stod(tok);
            },
        });
    }

    last_endp = prms.endpos;

    return *this;
}

Curve& Curve::save()
{
    if (in_db) {
        db->update(*this);
    } else {
        id = db->emplace(*this, id);
        in_db = true;
    }
    return *this;
}

Curve::entry_t Curve::get(seek_t read_head) const
{
    if (read_head > 1.0 || read_head < 0.0) {
        read_head = std::fmod(read_head, 1.0);
    }

    seek_t tab_dist = read_head * (tab_lenf - 1.0); 
    auto left_ndx = static_cast<decltype(tab_lenf)>(std::floor(tab_dist));
    entry_t left  = table.at(left_ndx);
    entry_t right =
        table.at(static_cast<decltype(tab_lenf)>(std::ceil(tab_dist)));
    seek_t rel_dist = tab_dist - left_ndx; 
    return left + rel_dist*(right - left);
}
