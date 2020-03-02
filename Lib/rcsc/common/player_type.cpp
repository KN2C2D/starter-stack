// -*-c++-*-

/*!
  \file player_type.cpp
  \brief heterogenious player parametor Source File
*/

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA, Hiroki SHIMORA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "player_type.h"

#include "player_param.h"
#include "server_param.h"
#include "stamina_model.h"

#include <rcsc/rcg/util.h>

#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cstring>
#include <cmath>

namespace rcsc {

/*-------------------------------------------------------------------*/
/*!

*/
PlayerType::PlayerType()
    : M_id( Hetero_Default )
{
    setDefault();
    initAdditionalParams();
}

/*-------------------------------------------------------------------*/
/*!

*/
PlayerType::PlayerType( const char * server_msg,
                        const double & version )
    : M_id( Hetero_Unknown )
{
    setDefault();

    if ( version >= 8.0 )
    {
        parseV8( server_msg );
    }
    else
    {
        parseV7( server_msg );
    }

    initAdditionalParams();
}

/*-------------------------------------------------------------------*/
/*!

*/
PlayerType::PlayerType( const rcg::player_type_t & from )
    : M_id( Hetero_Unknown )
{
    setDefault();

    M_id = rcg::nstohi( from.id );
    M_player_speed_max = rcg::nltohd( from.player_speed_max );
    M_stamina_inc_max = rcg::nltohd( from.stamina_inc_max );
    M_player_decay = rcg::nltohd( from.player_decay );
    M_inertia_moment = rcg::nltohd( from.inertia_moment );
    M_dash_power_rate = rcg::nltohd( from.dash_power_rate );
    M_player_size = rcg::nltohd( from.player_size );
    M_kickable_margin = rcg::nltohd( from.kickable_margin );
    M_kick_rand = rcg::nltohd( from.kick_rand );
    M_extra_stamina = rcg::nltohd( from.extra_stamina );
    M_effort_max = rcg::nltohd( from.effort_max );
    M_effort_min = rcg::nltohd( from.effort_min );

    if ( from.kick_power_rate != 0 ) M_kick_power_rate = rcg::nltohd( from.kick_power_rate );
    if ( from.foul_detect_probability != 0 ) M_foul_detect_probability = rcg::nltohd( from.foul_detect_probability );
    if ( from.catchable_area_l_stretch != 0 ) M_catchable_area_l_stretch = rcg::nltohd( from.catchable_area_l_stretch );

    initAdditionalParams();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerType::convertTo( rcg::player_type_t & to ) const
{
    to.id = rcg::hitons( M_id );
    to.player_speed_max = rcg::hdtonl( M_player_speed_max );
    to.stamina_inc_max = rcg::hdtonl( M_stamina_inc_max );
    to.player_decay = rcg::hdtonl( M_player_decay );
    to.inertia_moment = rcg::hdtonl( M_inertia_moment );
    to.dash_power_rate = rcg::hdtonl( M_dash_power_rate );
    to.player_size = rcg::hdtonl( M_player_size );
    to.kickable_margin = rcg::hdtonl( M_kickable_margin );
    to.kick_rand = rcg::hdtonl( M_kick_rand );
    to.extra_stamina = rcg::hdtonl( M_extra_stamina );
    to.effort_max = rcg::hdtonl( M_effort_max );
    to.effort_min = rcg::hdtonl( M_effort_min );

    to.kick_power_rate = rcg::hdtonl( M_kick_power_rate );
    to.foul_detect_probability = rcg::hdtonl( M_foul_detect_probability );
    to.catchable_area_l_stretch = rcg::hdtonl( M_catchable_area_l_stretch );
}

/*-------------------------------------------------------------------*/
/*!

*/
std::string
PlayerType::toStr() const
{
    std::ostringstream os;

    os << "(player_type "
       << "(id " << M_id << ')'
       << "(player_speed_max " << M_player_speed_max << ')'
       << "(stamina_inc_max " << M_stamina_inc_max << ')'
       << "(player_decay " << M_player_decay << ')'
       << "(inertia_moment " << M_inertia_moment << ')'
       << "(dash_power_rate " << M_dash_power_rate << ')'
       << "(player_size " << M_player_size << ')'
       << "(kickable_margin " << M_kickable_margin << ')'
       << "(kick_rand " << M_kick_rand << ')'
       << "(extra_stamina " << M_extra_stamina << ')'
       << "(effort_max " << M_effort_max << ')'
       << "(effort_min " << M_effort_min << ')'
       << "(kick_power_rate " << M_kick_power_rate << ')'
       << "(foul_detect_probability " << M_foul_detect_probability << ')'
       << "(catchable_area_l_stretch " << M_catchable_area_l_stretch << ')'
       << ')';

    return os.str();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerType::setDefault()
{
    const ServerParam & SP = ServerParam::instance();

    M_player_speed_max = SP.defaultPlayerSpeedMax();
    M_stamina_inc_max = SP.defaultStaminaIncMax();
    M_player_decay = SP.defaultPlayerDecay();
    M_inertia_moment = SP.defaultInertiaMoment();
    M_dash_power_rate = SP.defaultDashPowerRate();
    M_player_size = SP.defaultPlayerSize();
    M_kickable_margin = SP.defaultKickableMargin();
    M_kick_rand = SP.defaultKickRand();
    M_extra_stamina = SP.defaultExtraStamina();
    M_effort_max = SP.defaultEffortMax();
    M_effort_min = SP.defaultEffortMin();
    M_kick_power_rate = SP.kickPowerRate();
    M_foul_detect_probability = SP.foulDetectProbability();
    M_catchable_area_l_stretch = 1.0;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerType::parseV8( const char * msg )
{
    // read v8 protocol
    /*
      "(player_type (id 0) (player_speed_max 1.2) (stamina_inc_max 45)
      (player_decay 0.4) (inertia_moment 5)
      (dash_power_rate 0.006) (player_size 0.3)
      (kickable_margin 0.7) (kick_rand 0)
      (extra_stamina 0) (effort_max 1) (effort_min 0.6))"
      "(player_type (id 3) (player_speed_max 1.2) (stamina_inc_max 40.68)
      (player_decay 0.5308) (inertia_moment 8.27)
      (dash_power_rate 0.006432) (player_size 0.3)
      (kickable_margin 0.7002) (kick_rand 0.0001)
      (extra_stamina 98) (effort_max 0.804) (effort_min 0.404))";
    */

    int n_read = 0;

    char name[32];
    int id = 0;
    if ( std::sscanf( msg, " ( player_type ( %s %d ) %n ",
                      name, &id, &n_read ) != 2
         || n_read == 0
         || id < 0 )
    {
        std::cerr << "PlayerType id read error. "
                  << msg
                  << std::endl;
        return;
    }
    msg += n_read;

    M_id = id;

    int n_param = 0;
    while ( *msg != '\0' && *msg != ')' )
    {
        double val = 0.0;
        if ( std::sscanf( msg, " ( %s %lf ) %n ",
                          name, &val, &n_read ) != 2
             || n_read == 0 )
        {
            std::cerr << "PlayerType parameter read error: "
                      << msg << std::endl;
            break;
        }
        msg += n_read;

        if ( ! std::strcmp( name, "player_speed_max" ) )
        {
            M_player_speed_max = val;
        }
        else if ( ! std::strcmp( name, "stamina_inc_max" ) )
        {
            M_stamina_inc_max = val;
        }
        else if ( ! std::strcmp( name, "player_decay" ) )
        {
            M_player_decay = val;
        }
        else if ( ! std::strcmp( name, "inertia_moment" ) )
        {
            M_inertia_moment = val;
        }
        else if ( ! std::strcmp( name, "dash_power_rate" ) )
        {
            M_dash_power_rate = val;
        }
        else if ( ! std::strcmp( name, "player_size" ) )
        {
            M_player_size = val;
        }
        else if ( ! std::strcmp( name, "kickable_margin" ) )
        {
            M_kickable_margin = val;
        }
        else if ( ! std::strcmp( name, "kick_rand" ) )
        {
            M_kick_rand = val;
        }
        else if ( ! std::strcmp( name, "extra_stamina" ) )
        {
            M_extra_stamina = val;
        }
        else if ( ! std::strcmp( name, "effort_max" ) )
        {
            M_effort_max = val;
        }
        else if ( ! std::strcmp( name, "effort_min" ) )
        {
            M_effort_min = val;
        }
        else if ( ! std::strcmp( name, "kick_power_rate" ) )
        {
            M_kick_power_rate = val;
        }
        else if ( ! std::strcmp( name, "foul_detect_probability" ) )
        {
            M_foul_detect_probability = val;
        }
        else if ( ! std::strcmp( name, "catchable_area_l_stretch" ) )
        {
            M_catchable_area_l_stretch = val;
        }
        else
        {
            std::cerr << "player_type: param name error " << msg << std::endl;
            break;
        }

        ++n_param;
    }

    //std::cerr << "  read " << n_param << " params" << endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerType::parseV7( const char * msg )
{
    // read v7 protocol
    // read param value only, no param name
    /*
      from rcssserver/src/hetroplayer.C
      std::ostream& toStr(std::ostream& o,
      const PlayerTypeSensor_v7::data_t& data)
      {
      return o << "(player_type " << data.M_id << " "
      << data.M_player_speed_max << " "
      << data.M_stamina_inc_max << " "
      << data.M_player_decay << " "
      << data.M_inertia_moment << " "
      << data.M_dash_power_rate << " "
      << data.M_player_size << " "
      << data.M_kickable_margin << " "
      << data.M_kick_rand << " "
      << data.M_extra_stamina << " "
      << data.M_effort_max << " "
      << data.M_effort_min
      << ")";
      }
    */

    std::istringstream msg_strm( msg );
    std::string tmp;

    msg_strm >> tmp // skip "(player_type"
             >> M_id
             >> M_player_speed_max
             >> M_stamina_inc_max
             >> M_player_decay
             >> M_inertia_moment
             >> M_dash_power_rate
             >> M_player_size
             >> M_kickable_margin
             >> M_kick_rand
             >> M_extra_stamina
             >> M_effort_max
             >> M_effort_min;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerType::initAdditionalParams()
{
    const ServerParam & SP = ServerParam::i();

    M_kickable_area = playerSize() + kickableMargin() + SP.ballSize();

    /////////////////////////////////////////////////////////////////////
    const double catch_stretch_length_x = ( catchAreaLengthStretch() - 1.0 ) * SP.catchAreaLength();
    const double catch_length_min_x = SP.catchAreaLength() - catch_stretch_length_x;
    const double catch_length_max_x = SP.catchAreaLength() + catch_stretch_length_x;

    const double catch_half_width2 = std::pow( SP.catchAreaWidth() / 2.0, 2 );

    M_reliable_catchable_dist = std::sqrt( std::pow( catch_length_min_x, 2 )
                                           + catch_half_width2 );
    M_max_catchable_dist = std::sqrt( std::pow( catch_length_max_x, 2 )
                                      + catch_half_width2 );

    ///////////////////////////////////////////////////////////////////
    double accel = SP.maxDashPower() * dashPowerRate() * effortMax();

    // see also soccer_math.h
    M_real_speed_max = accel / ( 1.0 - playerDecay() ); // sum inf geom series
    if ( M_real_speed_max > playerSpeedMax() )
    {
        M_real_speed_max = playerSpeedMax();
    }

    ///////////////////////////////////////////////////////////////////
    M_player_speed_max2 = playerSpeedMax() * playerSpeedMax();
    M_real_speed_max2 = realSpeedMax() * realSpeedMax();

    /////////////////////////////////////////////////////////////////////
    double speed = 0.0;
    double dash_power = SP.maxDashPower();
    StaminaModel stamina_model;
    stamina_model.init( *this );

    double reach_dist = 0.0;

    M_cycles_to_reach_max_speed = -1;

    M_dash_distance_table.clear();
    M_dash_distance_table.reserve( 50 );

    for ( int counter = 1; counter <= 50; ++counter )
    {
        if ( speed + accel > playerSpeedMax() )
        {
            accel = playerSpeedMax() - speed;
            dash_power = std::min( SP.maxDashPower(),
                                   accel / ( dashPowerRate() * stamina_model.effort() ) );
        }

        speed += accel;

        reach_dist += speed;

        M_dash_distance_table.push_back( reach_dist );

        if ( M_cycles_to_reach_max_speed < 0
             && speed >= realSpeedMax() - 0.01 )
        {
            M_cycles_to_reach_max_speed = counter;
        }

        speed *= playerDecay();

        stamina_model.simulateDash( *this, dash_power );

        if ( stamina_model.stamina() <= 0.0 )
        {
            break;
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
double
PlayerType::reliableCatchableDist() const
{
    if ( ServerParam::i().catchProbability() < 1.0 )
    {
        return 0.0;
    }
    else
    {
        return M_reliable_catchable_dist;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
double
PlayerType::reliableCatchableDist( const double prob ) const
{
    if ( prob > 1.0 )
    {
        std::cerr << "internal error at PlayerType::reliableCatchableDist(): "
                  << "probability " << prob << " too big" << std::endl;

        return reliableCatchableDist();
    }
    else if ( prob < 0.0 )
    {
        std::cerr << "internal error at PlayerType::reliableCatchableDist(): "
                  << "probability " << prob << " too small" << std::endl;

        return maxCatchableDist();
    }

    const ServerParam & SP = ServerParam::i();

    const double target_prob = prob / SP.catchProbability();
    if ( target_prob > 1.0 )
    {
        return 0.0;
    }

    const double catch_stretch_length_x = ( catchAreaLengthStretch() - 1.0 ) * SP.catchAreaLength();
    const double catch_length_min_x = SP.catchAreaLength() - catch_stretch_length_x;

    const double dist_x = catch_length_min_x + ( catch_stretch_length_x * 2.0 * ( 1.0 - target_prob ) );

    const double diagonal_length = std::sqrt( std::pow( dist_x, 2 )
                                              + std::pow( SP.catchAreaWidth() / 2.0, 2 ) );
    return diagonal_length;
}
/*-------------------------------------------------------------------*/
/*!

*/
double
PlayerType::getCatchProbability( const double dist ) const
{
    const ServerParam & SP = ServerParam::i();

    if ( dist < M_reliable_catchable_dist )
    {
        if ( dist < 0.0 )
        {
            std::cerr << "internal error at PlayerType::getCatchProbability(dist): negative distance "
                      << dist << std::endl;
        }

        return SP.catchProbability();
    }
    else if ( dist > M_max_catchable_dist )
    {
        return 0.0;
    }

    const double catch_stretch_length_x = ( catchAreaLengthStretch() - 1.0 ) * SP.catchAreaLength();
    const double catch_length_min_x = SP.catchAreaLength() - catch_stretch_length_x;

    const double dist_x = std::sqrt( std::pow( dist, 2 )
                                     - std::pow( SP.catchAreaWidth() / 2.0, 2 ) );

    const double fail_prob = ( dist_x - catch_length_min_x ) / ( catch_stretch_length_x * 2.0 );

    return ( 1.0 - fail_prob ) * SP.catchProbability();
}



/*-------------------------------------------------------------------*/
/*!

*/
int
PlayerType::getMaxDashCyclesSavingRecovery( const double & dash_power,
                                            const double & current_stamina,
                                            const double & current_recovery ) const
{
    double available_stamina
        = current_stamina
        - ServerParam::i().recoverDecThrValue()
        - 1.0;
    double used_stamina = ( dash_power > 0.0 ? dash_power : dash_power * -2.0 );

    available_stamina -= used_stamina; // buffer for last one dash
    if ( available_stamina < 0.0 )
    {
        return 0;
    }

    double one_cycle_consumpation
        = used_stamina - ( staminaIncMax() * current_recovery );
    return static_cast< int >
        ( std::floor( available_stamina / one_cycle_consumpation ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
#if 0
int
PlayerType::maxDashCyclesWith( const double & stamina ) const
{
    if ( stamina <= 0.0 )
    {
        return 0;
    }

    std::vector< double >::const_iterator
        it = std::lower_bound( M_stamina_table.begin(),
                               M_stamina_table.end(),
                               stamina );

    if ( it != M_dash_distance_table.end() )
    {
        return static_cast< int >( it - M_stamina_table.begin() );
    }

    if ( stamina < M_stamina_table.front() )
    {
        return 0;
    }

    return M_stamina_table.size();
}
#endif

/*-------------------------------------------------------------------*/
/*!

*/
#if 0
double
PlayerType::consumedStaminaAfterNrDash( const int n_dash ) const
{
    if ( 0 <= n_dash && n_dash < (int)M_stamina_table.size() )
    {
        return M_stamina_table[n_dash];
    }

    return M_stamina_table.back();
}
#endif

/*-------------------------------------------------------------------*/
/*!

*/
int
PlayerType::cyclesToReachMaxSpeed( const double & dash_power ) const
{
    double accel = std::fabs( dash_power ) * dashPowerRate() * effortMax();
    double speed_max = accel / ( 1.0 - playerDecay() );
    if ( speed_max > playerSpeedMax() )
    {
        speed_max = playerSpeedMax();
    }

    double decn = 1.0 - ( ( speed_max - 0.01 ) * ( 1.0 - playerDecay() ) / accel );
    return static_cast< int >( std::ceil( std::log( decn )
                                          / std::log( playerDecay() ) ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
int
PlayerType::cyclesToReachDistance( const double & dash_dist ) const
{
    if ( dash_dist <= 0.001 )
    {
        return 0;
    }

    std::vector< double >::const_iterator
        it = std::lower_bound( M_dash_distance_table.begin(),
                               M_dash_distance_table.end(),
                               dash_dist - 0.001 );

    if ( it != M_dash_distance_table.end() )
    {
        return ( static_cast< int >
                 ( std::distance( M_dash_distance_table.begin(), it ) )
                 + 1 ); // is it necessary?
    }

    double rest_dist = dash_dist - M_dash_distance_table.back();
    int cycle = M_dash_distance_table.size();

    cycle += static_cast< int >( std::ceil( rest_dist / realSpeedMax() ) );

    return cycle;
}

/*-------------------------------------------------------------------*/
/*!

*/
double
PlayerType::getDashPowerToKeepMaxSpeed( const double & effort ) const
{
    // required accel in 1 step to keep max speed
    double required_power = playerSpeedMax() * ( 1.0 - playerDecay() );
    // required dash power to keep max speed
    required_power /= ( effort * dashPowerRate() );

    return std::min( required_power, ServerParam::i().maxDashPower() );
}

/*-------------------------------------------------------------------*/
/*!

*/
double
PlayerType::kickRate( const double & ball_dist,
                      const double & dir_diff ) const
{
    return (
            kickPowerRate() //ServerParam::i().kickPowerRate()
            * ( 1.0
                - 0.25 * std::fabs( dir_diff ) / 180.0
                - ( 0.25 * ( ball_dist - ServerParam::i().ballSize() - playerSize() )
                    / kickableMargin() ) )
            );
}

/*-------------------------------------------------------------------*/
/*!

*/
double
PlayerType::dashRate( const double & effort,
                      const double & rel_dir ) const
{
    return dashRate( effort ) * ServerParam::i().dashDirRate( rel_dir );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
PlayerType::normalizeAccel( const Vector2D & velocity,
                            const AngleDeg & accel_angle,
                            double * accel_mag ) const
{
    // if ( *accel_mag > SP.playerAccelMax() )
    //     *accel_mag = SP.playerAccelMax();

    Vector2D dash_move = velocity;
    dash_move += Vector2D::polar2vector( *accel_mag, accel_angle ); // add dash accel

    if ( dash_move.r2() > playerSpeedMax2() + 0.0001 )
    {
        Vector2D rel_vel = velocity.rotatedVector( -accel_angle );
        // sqr(rel_vel.y) + sqr(max_dash_x) == sqr(max_speed);
        // accel_mag = dash_x - rel_vel.x;
        double max_dash_x = std::sqrt( playerSpeedMax2()
                                       - rel_vel.y * rel_vel.y );
        *accel_mag = max_dash_x - rel_vel.x;
        return true;
    }
    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
PlayerType::normalizeAccel( const Vector2D & velocity,
                            Vector2D * accel ) const
{
    // double tmp = accel_mag->r();
    // if ( tmp > SP.playerAccelMax() )
    //     *accel *= SP.playerAccelMax() / tmp;

    if ( ( velocity + (*accel) ).r2() > playerSpeedMax2() + 0.0001 )
    {
        Vector2D rel_vel = velocity.rotatedVector( -accel->th() );
        // sqr(rel_vel.y) + sqr(max_dash_x) == sqr(max_speed);
        // accel_mag = dash_x - rel_vel.x;
        double max_dash_x = std::sqrt( playerSpeedMax2()
                                       - rel_vel.y * rel_vel.y );
        accel->setLength( max_dash_x - rel_vel.x );
        return true;
    }
    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
std::ostream &
PlayerType::print( std::ostream & os ) const
{
    os << "player_type id : " << id()
       << "\n  player_speed_max : " << playerSpeedMax()
       << "\n  stamina_inc_max :  " << staminaIncMax()
       << "\n  player_decay : "     << playerDecay()
       << "\n  inertia_moment : "   << inertiaMoment()
       << "\n  dash_power_rate : "  << dashPowerRate()
       << "\n  player_size : "      << playerSize()
       << "\n  kickable_margin : "  << kickableMargin()
       << "\n  kick_rand : "        << kickRand()
       << "\n  extra_stamina : "    << extraStamina()
       << "\n  effort_max : "       << effortMax()
       << "\n  effort_min : "       << effortMin()
       << std::endl;
    return os;
}



///////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------*/
/*!

*/
PlayerTypeSet &
PlayerTypeSet::instance()
{
    static PlayerTypeSet S_instance;
    return S_instance;
}

/*-------------------------------------------------------------------*/
/*!

*/
PlayerTypeSet::PlayerTypeSet()
{
    resetDefaultType();
}

/*-------------------------------------------------------------------*/
/*!

*/
PlayerTypeSet::~PlayerTypeSet()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerTypeSet::resetDefaultType()
{
    PlayerType default_type;
    insert( default_type );
    M_dummy_type = default_type;
}

/*-------------------------------------------------------------------*/
/*!
  return pointer to param
*/
void
PlayerTypeSet::insert( const PlayerType & param )
{
    if ( static_cast< int >( M_player_type_map.size() )
         >= PlayerParam::i().playerTypes() )
    {
        std::cerr << "# of player type over flow" << std::endl;
        return;
    }

    // insert new type
    M_player_type_map.insert( std::make_pair( param.id(), param ) );


    if ( static_cast< int >( M_player_type_map.size() )
         == PlayerParam::i().playerTypes() )
    {
        createDummyType();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerTypeSet::createDummyType()
{
    for ( PlayerTypeMap::iterator it = M_player_type_map.begin();
          it != M_player_type_map.end();
          ++it )
    {
        if ( it->second.realSpeedMax() > M_dummy_type.realSpeedMax() )
        {
            M_dummy_type = it->second;
        }
        else if ( std::fabs( it->second.realSpeedMax() - M_dummy_type.realSpeedMax() ) < 0.01
                  && it->second.cyclesToReachMaxSpeed() < M_dummy_type.cyclesToReachMaxSpeed() )
        {
            M_dummy_type = it->second;
        }
    }
}

/*-------------------------------------------------------------------*/
/*!
  return pointer to param
*/
const
PlayerType *
PlayerTypeSet::get( const int id ) const
{
    if ( id == Hetero_Unknown )
    {
        return &M_dummy_type;
    }
    else
    {
        PlayerTypeMap::const_iterator it = M_player_type_map.find( id );
        if ( it != M_player_type_map.end() )
        {
            return &( it->second );
        }
    }

    std::cerr << "PlayerTypeSet: get : player type id error " << id << std::endl;
    return static_cast< PlayerType * >( 0 );
}

/*-------------------------------------------------------------------*/
/*!

*/
std::ostream &
PlayerTypeSet::print( std::ostream & os ) const
{
    os << "All Player Types: n";

    PlayerTypeMap::const_iterator it = M_player_type_map.begin();
    while ( it != M_player_type_map.end() )
    {
        it->second.print( os );
        ++it;
    }
    return os;
}

}
