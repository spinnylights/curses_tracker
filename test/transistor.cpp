#include <doctest.h>

#include <type_traits>

//double emit(double base, double collector)
//{
//    if (base < 0.0) {
//        base = 0.0;
//    } else if (base > 1.0) {
//        base = 1.0;
//    }
//
//    return base*collector;
//}
//
//double emit(bool base, double collector)
//{
//    if (base) {
//        return collector;
//    } else {
//        return 0.0;
//    }
//}
//
//// emit(false, false) -> false
//// emit(true,  false) -> false
//// emit(false, true)  -> false
//// emit(true,  true)  -> true
//bool emit(bool base, bool collector)
//{
//    if (base) {
//        return collector;
//    } else {
//        return false;
//    }
//}

template<typename BaseT=bool, typename CollectorT=double>
CollectorT emit(BaseT base, CollectorT collector)
{
    if (std::is_floating_point<BaseT>::value) {
        if (base < 0.0) {
            base = 0.0;
        } else if (base > 1.0) {
            base = 1.0;
        }

        return base*collector;
    } else {
        if (base) {
            return collector;
        } else {
            return CollectorT(0);
        }
    }
}

TEST_CASE("transistor") {
    CHECK(emit(false, false) == false);
    CHECK(emit(true,  false) == false);
    CHECK(emit(false, true)  == false);
    CHECK(emit(true,  true)  == true);

    CHECK(emit(0.25,  10.0)  == 2.5);

    CHECK(emit(false, 0.75)  == 0.0);
    CHECK(emit(true,  0.75)  == 0.75);

    bool   sigd = true;
    double siga = 10.0;

    SUBCASE("NOR") {
        CHECK(emit(false + false, sigd) == false);
        CHECK(emit(true  + false, sigd) == sigd);
        CHECK(emit(false + true,  sigd) == sigd);
        CHECK(emit(true  + true,  sigd) == sigd);

        CHECK(emit(false + false, siga) == 0.0);
        CHECK(emit(true  + false, siga) == siga);
        CHECK(emit(false + true,  siga) == siga);
        CHECK(emit(true  + true,  siga) == siga);

        CHECK(emit(0.0  + 0.2, siga) == siga*0.2);
        CHECK(emit(1.0  + 1.0, siga) == siga);
        CHECK(emit(-1.0 - 1.0, siga) == 0.0);
    }

    SUBCASE("AND") {
        CHECK(emit(false, emit(false, sigd)) == false);
        CHECK(emit(false, emit(true,  sigd)) == false);
        CHECK(emit(true,  emit(false, sigd)) == false);
        CHECK(emit(true,  emit(true,  sigd)) == sigd);

        CHECK(emit(false, emit(false, siga)) == 0.0);
        CHECK(emit(false, emit(true,  siga)) == 0.0);
        CHECK(emit(true,  emit(false, siga)) == 0.0);
        CHECK(emit(true,  emit(true,  siga)) == siga);

        CHECK(emit(0.0, emit(0.5, siga)) == 0.0);
        CHECK(emit(0.5, emit(0.0, siga)) == 0.0);
        CHECK(emit(0.5, emit(0.5, siga)) == siga*0.25);
        CHECK(emit(4.0, emit(2.0, siga)) == siga);
    }

    SUBCASE("NAND") {
        // nand(false, false, sigd) -> sigd
        // nand(true,  false, sigd) -> sigd
        // nand(false, true,  sigd) -> sigd
        // nand(true,  true,  sigd) -> false
        CHECK(emit(emit(false, sigd), false) == false);
        CHECK(emit(emit(false, sigd), true) == false);
        CHECK(emit(false, emit(true,  sigd)) == false);
        CHECK(emit(true,  emit(false, sigd)) == false);
        CHECK(emit(true,  emit(true,  sigd)) == sigd);
    }
}
