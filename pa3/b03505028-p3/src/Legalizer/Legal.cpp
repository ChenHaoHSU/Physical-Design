#include "Legal.h"
#include "arghandler.h"
#include "GnuplotLivePlotter.h"
#include "GnuplotMatrixPlotter.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <climits>
#include <list>
#include <set>
#include <cassert>
#include <ctime>
using namespace std;

struct sortCellIdxX
{
    vector<CPoint> m_globalLocations;
    sortCellIdxX(vector<CPoint>& v) { m_globalLocations = v; }
    bool operator() (const int& a, const int& b)
    {
        return m_globalLocations[ a ].x < m_globalLocations[ b ].x;
    }
};


bool sortPair(const IntDbl& a, const IntDbl& b)
{
    return a.second < b.second;
}


float getTime(const time_t& start)
{
    time_t end = clock();
    return (float)( end - start ) / CLOCKS_PER_SEC;
}


bool CLegal::solve()
{
    // TODO: edit your code HERESSSS
    // Note:
    //      1. You should save your legal solution into m_bestLocations, 
    //         and call setLegalResult() to upload it into Placement.
    //      2. Run check() to make sure that the solution is lega.
    //      3. Feel free to add any class, function, and/or data member.
    // Good luck!
    time_t start = clock();

    saveGlobalResult();

    build_m_cell_order();

    for(unsigned i = 0, n = m_cell_order.size(); i < n; ++i)
    {
        // cout << i << " / " << n << flush << '\r';

        const int& curIdx = m_cell_order[ i ];
        const double& curY = m_globalLocations[ curIdx ].y;

        int nearestRowIdx = (int)( ( curY - m_site_bottom ) / m_site_height );
        double c_best = INT_MAX;
        int r_best = -1;

        if( nearestRowIdx < 0 ) nearestRowIdx = 0;

        for(unsigned rowIdx = nearestRowIdx; rowIdx < rowModuleIdx.size(); ++rowIdx)
        {
            if( checkBreak( rowIdx, curIdx, c_best ) ) break; // branch and bound
            insertCell( rowIdx, curIdx );
            double c = placeRow( rowIdx );
            if( c < c_best )
            {
                c_best = c;
                r_best = rowIdx;
            }
            removeLastCell( rowIdx );
        }

        for(unsigned rowIdx = nearestRowIdx; rowIdx >= 0; --rowIdx)
        {
            if( checkBreak( rowIdx, curIdx, c_best ) ) break; // branch and bound
            insertCell( rowIdx, curIdx );
            double c = placeRow( rowIdx );
            if( c < c_best )
            {
                c_best = c;
                r_best = rowIdx;
            }
            removeLastCell( rowIdx );
            if( rowIdx == 0) break;
        }

        insertCell( r_best, curIdx );
        placeRow( r_best , false );
    }

    setLegalResult();

// print legalization time ( my legalization running time )
    cout << endl << endl << "////// Running Time //////" << endl;
    cout << "Legalization time: " << getTime(start) << " seconds." << endl;


// check the legalization result and calculate displacement
    cout << endl << "////// Result //////" << endl;
    if( check() )
    {
        cout<< "total displacement: " << totalDisplacement() << endl;
    }
    else
        return false;

    return true;

}


void CLegal::build_m_cell_order()
{
    std::sort( m_pair_idx_globalX.begin(), m_pair_idx_globalX.end(), sortPair );

    for(unsigned i = 0; i < m_pair_idx_globalX.size(); ++i)
    {
        m_cell_order.push_back( m_pair_idx_globalX[ i ].first );
    }

    assert( m_cell_order.size() == m_pair_idx_globalX.size() );
}


inline bool CLegal::checkBreak(const int& r_idx, const int& curId, const double& c_best) const 
{
    const double& rowY = m_free_sites[ r_idx ].y();
    const double& globalY = m_globalLocations[ curId ].y;

    return ( fabs( rowY - globalY ) > c_best );   
}


void CLegal::insertCell(const int& r_idx, const int& m_idx)
{
    rowModuleIdx[ r_idx ].push_back( m_idx );
    rowTotalWidth[ r_idx ] += _placement.module( m_idx ).width();
}


double CLegal::placeRow(const int& r_idx, bool isTrial)
{
    static int i = 0;

    if( rowTotalWidth[ r_idx ] > max_x - min_x )
        return INT_MAX;

    if( isTrial )
    {
        rowClusters[ r_idx ].clear();
        rowClusters[ r_idx ] = bestClusters[ r_idx ];

        for(unsigned i = rowModuleIdx[ r_idx ].size() - 1; i < rowModuleIdx[ r_idx ].size(); ++i)
        {
            const int& row_m_idx = rowModuleIdx[ r_idx ][ i ];
            const int& globalX = m_globalLocations[ row_m_idx ].x;

            if( i == 0 )
            {
                Cluster newC(0, 0, 0);
                newC._x = globalX; 
                newC._n_first = i;
                addCell( r_idx, newC, i );
                rowClusters[ r_idx ].push_back( newC );
                continue;
            }

            Cluster& lastC = rowClusters[ r_idx ].back();

            if( lastC._x + lastC._w <= globalX)
            {
                Cluster newC(0, 0, 0);
                newC._x = globalX; 
                newC._n_first = i;
                addCell( r_idx, newC, i );
                rowClusters[ r_idx ].push_back( newC );
            }
            else
            {
                int lastCIdx = rowClusters[ r_idx ].size() - 1;
                addCell( r_idx, lastC, i );
                collapse( r_idx, lastCIdx );
            }

        }
    }
    else
    {
        bestClusters[ r_idx ] = rowClusters[ r_idx ];
        c_cost[ r_idx ] = getDisRow( r_idx );
    }

    i = 0;
    for(unsigned clusterIdx = 0; clusterIdx < rowClusters[ r_idx ].size(); ++clusterIdx)
    {
        Cluster& curCluster = rowClusters[ r_idx ][ clusterIdx ];
        double x = curCluster._x;
        
        for(; i <= curCluster._n_last; ++i)
        {
            const int& row_m_idx = rowModuleIdx[ r_idx ][ i ];
            m_bestLocations[ row_m_idx ].x = x;
            x = x + _placement.module( row_m_idx ).width();
            m_bestLocations[ row_m_idx ].y = m_free_sites[ r_idx ].y();
        }
    }


    return ( isTrial ? ( getDisRow( r_idx ) - c_cost[ r_idx ] ) : 0.0 );
}


double CLegal::getDisRow(const int& r_idx)
{
    static double totaldis = 0;
    static double delta_x = 0;
    static double delta_y = 0;

    totaldis = 0;
    for(unsigned i = 0; i < rowModuleIdx[ r_idx ].size(); ++i )
    {
        const int& row_m_idx = rowModuleIdx[ r_idx ][ i ];
        const CPoint& curPos = m_bestLocations[ row_m_idx ];
        const CPoint& globalPos = m_globalLocations[ row_m_idx ];

        delta_x = fabs( curPos.x - globalPos.x );
        delta_y = fabs( curPos.y - globalPos.y );

        totaldis += ( delta_x + delta_y );
    }
    return totaldis;
}


void CLegal::addCell(const int& r_idx, Cluster& c, const int& i)
{ 
    const int& row_m_idx = rowModuleIdx[ r_idx ][ i ];
    const double& globalX = m_globalLocations[ row_m_idx ].x;
    const double curW = _placement.module( row_m_idx ).width();

    c._n_last = i;
    c._e = ( c._e + 1);  // the same weight 1
    c._q = ( c._q + ( globalX - c._w ) );
    c._w = ( c._w + curW );
}


void CLegal::addCluster(const int& r_idx, const int& live, const int& die)
{
    // liveC is predecessor of dieC !!
    Cluster& dieC = rowClusters[ r_idx ][ die ];   // dying cluster
    Cluster& liveC = rowClusters[ r_idx ][ live ]; // alive cluster

    liveC._n_last = ( dieC._n_last );
    liveC._e = ( liveC._e + dieC._e );
    liveC._q = ( liveC._q + dieC._q - ( dieC._e * liveC._w ) );
    liveC._w = ( liveC._w + dieC._w );
}


void CLegal::collapse(const int& r_idx, const int& c_idx)
{
    Cluster& curC = rowClusters[ r_idx ][ c_idx ];
    curC._x = ( curC._q / curC._e );  // optimal position

    // check if the cluster is out of the boundary
    //// min_x boundary
    if( curC._x < min_x)
    {
        curC._x = ( min_x );
    }
    //// max_x boundary
    if( curC._x + curC._w > max_x)
    {
        curC._x = ( max_x - curC._w );
    }

    if( c_idx > 0 ) // if predecessor exists
    {
        const Cluster& preC = rowClusters[ r_idx ][ c_idx - 1 ];
        if( preC._x + preC._w > curC._x )
        {
            addCluster( r_idx, ( c_idx - 1 ), c_idx );
            rowClusters[ r_idx ].pop_back(); // remove die cluster
            collapse( r_idx, ( c_idx - 1 ) );
        }
    }

    return;
}

void CLegal::removeLastCell(const int& r_idx)
{
    rowTotalWidth[ r_idx ] -= _placement.module( rowModuleIdx[ r_idx ].back() ).width();
    rowModuleIdx[ r_idx ].pop_back();
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

bool sortModule( Module* a, Module* b)
{
    return a->x() < b->x();
}

bool CLegal::check()
{
    cout << "start check" << endl;
    int notInSite=0;
    int notInRow=0;
    int overLap=0;

    ///////////////////////////////////////////////////////
    //1.check all standard cell are on row and in the core region
    //////////////////////////////////////////////////////////
    for(unsigned int i=0; i<_placement.numModules(); ++i)
    {
        Module& module = _placement.module(i);
        if(module.isFixed()) continue;
        double curX = module.x();
        double curY = module.y();

        double res = ( curY - m_site_bottom ) / _placement.getRowHeight();
        //cout << curY << " " << res << endl;
        int ires = (int) res;
        if( (m_site_bottom + _placement.getRowHeight() * ires) != curY )
        {
                cerr<<"\nWarning: cell:"<<i<<" is not on row!!";
                ++notInRow;
        }
        if( (curY<_placement.boundaryBottom()) || (curX<_placement.boundaryLeft())||
                ( (curX+module.width())>_placement.boundaryRight()) ||
                ( (curY+module.height())>_placement.boundaryTop()) )
        {
            cerr<<"\nWarning: cell:"<<i<<" is not in the core!!";
            ++notInSite;
        }
    }

    ///////////////////////////////////////////
    //2. row-based overlapping checking
    ///////////////////////////////////////////

    Rectangle chip = _placement.rectangleChip();
    const double &rowHeight = _placement.getRowHeight();
    unsigned numRows = _placement.numRows();
    vector< vector<Module*> > rowModules( numRows, vector<Module*>( 0 ) );
    for(unsigned int i=0; i<_placement.numModules(); ++i)
    {
        Module& module = _placement.module(i);
        double curY = m_bestLocations[i].y;

        if( module.area() == 0 ) continue;
        if( module.isFixed() ) continue;

        double yLow = curY - chip.bottom(), yHigh= curY + module.height() - chip.bottom();
        size_t low = floor( yLow / rowHeight ), high = floor(yHigh / rowHeight);
        if( fabs( yHigh - rowHeight * floor(yHigh / rowHeight) ) < 0.01 ){ --high; }

        for( size_t i = low; i <= high; ++i ){ rowModules[ i ].push_back( &module ); }
    }
    for( size_t i = 0; i < numRows; ++i )
    {
        vector<Module*> &modules = rowModules[i];
        sort(modules.begin(), modules.end(), sortModule);
        if( modules.size() < 1 ) { continue; }
        for( size_t j = 0; j < modules.size() - 1; ++j ){
            Module &mod = *modules[ j ];
            if(mod.isFixed()) continue;
            size_t nextId = j+1;
            while( mod.x() + mod.width() - modules[ nextId ]->x() > 0.01 ){
                Module &modNext = *modules[ nextId ];
                if( mod.x() + mod.width() - modules[ nextId ]->x() > 0.01 ){
                    ++overLap;
                    cout << mod.name() << " overlap with " << modNext.name() << endl;
                    cout << mod.x() << " " << mod.y() << " w = " << mod.width() << endl;
                    cout << modNext.x() << " " << modNext.y() << " w = " << modNext.width() << endl;
                }
                ++nextId; if( nextId == modules.size() ) { break; }
            }
        }
    }

    cout << endl <<
            "  # row error: "<<notInRow<<
            "\n  # site error: "<<notInSite<<
            "\n  # overlap error: "<<overLap<< endl;
    //cout << "end of check" << endl;

    if( notInRow!=0 || notInSite!=0 || overLap!=0 )
    {
        cout <<"Check failed!!" << endl;
        return false;
    }
    else
    {
        cout <<"Check success!!" << endl;
        return true;
    }
}

double CLegal::totalDisplacement()
{
    double totaldis = 0;
    for( unsigned  moduleId = 0 ; moduleId < _placement.numModules() ; moduleId++ )
    {
        Module& curModule = _placement.module(moduleId);
        double x = curModule.x();
        double y = curModule.y();

        totaldis += CPoint::Distance(m_globalLocations[moduleId] , CPoint( x, y ));
    }
    return totaldis;
}

CLegal::CLegal( Placement& placement  ) :
    _placement( placement )
{

    //Compute average cell width
    int cell_count = 0;
    double total_width = 0;
    //double max_height = 0.0;
    m_max_module_height = 0.0;
    m_max_module_width = 0.0;
    for( unsigned  moduleId = 0 ; moduleId < placement.numModules() ; moduleId++ )
    {
        Module& curModule = placement.module(moduleId);

        m_max_module_height = max( m_max_module_height, curModule.height() );
        m_max_module_width = max( m_max_module_width, curModule.width() );
    //Do not include fixed cells and macros
        if( curModule.isFixed() || curModule.height() > placement.getRowHeight() )
        continue;

        cell_count++;
        total_width += curModule.width();

    }

    m_average_cell_width = total_width / cell_count;

    m_free_sites = placement.m_sites;
    m_site_bottom = m_free_sites.front().y();
    m_site_height = m_free_sites.front().height();

    //initalize m_origLocations and m_bestLocations
    m_bestLocations.resize( placement.numModules() );
    m_globalLocations.resize( placement.numModules() );
    m_chip_left_bound = placement.rectangleChip().left();


    //////////////////////////
    min_x = m_free_sites.front().x();
    max_x = m_free_sites.front().x() + m_free_sites.front().width();

    rowClusters.resize( placement.numRows() );
    bestClusters.resize( placement.numRows() );
    rowModuleIdx.resize( placement.numRows() );
    rowTotalWidth.resize( placement.numRows() );
    c_cost.resize( placement.numRows() );

    for(unsigned i = 0; i < rowTotalWidth.size(); ++i)
        rowTotalWidth[ i ] = 0.0;

    for(unsigned i = 0; i < c_cost.size(); ++i)
        c_cost[ i ] = INT_MAX;
    /////////////////////////

}

void CLegal::saveGlobalResult()
{
    for (unsigned moduleId = 0; moduleId < _placement.numModules(); moduleId++)
    {
        Module &curModule = _placement.module(moduleId);
        double x = curModule.x();
        double y = curModule.y();

        m_globalLocations[moduleId] = CPoint( x, y );
        m_bestLocations[moduleId] = CPoint( x, y );

        //////////////////////////////////////
        //Do not include fixed cells and macros
        if( curModule.isFixed() || curModule.height() > _placement.getRowHeight() )
        continue;

        // const double& gX = m_globalLocations[ moduleId ].x;
        const double& gX = m_globalLocations[ moduleId ].x + ( curModule.width() / 2 );
        m_pair_idx_globalX.push_back( IntDbl((int)moduleId, gX) );
        //////////////////////////////////////

    }
}

void CLegal::setLegalResult()
{
    for (unsigned moduleId = 0; moduleId < _placement.numModules(); moduleId++)
    {
        Module &curModule = _placement.module(moduleId);
        curModule.setPosition(m_bestLocations[moduleId].x,m_bestLocations[moduleId].y);
    }
}


