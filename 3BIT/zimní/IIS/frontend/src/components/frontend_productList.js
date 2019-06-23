import React, {Component} from 'react';
import {Link, Redirect} from "react-router-dom";
import axios from 'axios';
//
import Article from './frontend_productListItem.js'
import Loader from './loader.js'
//
import {CATEGORIES, API_MAIN, API_BASE, ROUTE_PREFIX, TOKEN_NAME, on403} from '../const.js'
//
import hero from'../assets/hero.jpg';


export default class ProductList extends Component {
    constructor(props) {
        super(props)
        this.state = {
            categoryId: props.match ? props.match.params.categoryId : '',
            data: [],
            filteredData: [],
            isLoading: false,
            error: null,
        };

        this.componentType = props.productListType
    }

    componentDidMount() {
        this.setState({ isLoading: true });
        // ziskame data pro kategorie
        if (this.componentType === 'category') {
            this.fetchData(`${API_BASE}categories/${this.state.categoryId}`)
        } 
        // vsechny data
        else {
            this.fetchData(API_MAIN)
        }
    }

    async fetchData(url) {
        try {
            const fetchedData = await axios(url, {
                timeout: 2000,
                headers: {
                    token: JSON.parse(localStorage.getItem(TOKEN_NAME))['token']
                }
            })
            this.setState({
                data: fetchedData.data,
                filteredData: fetchedData.data,
                isLoading: false
            })
        } catch (error) {
            if (error.response.status === 403) {
                on403()
            }
            this.setState({
                error: error,
                isLoading: false
            })
            
        }
    }

    filterItems(newVal) {
        this.setState({filteredData: this.state.data.filter(cur => cur.kategorie === newVal)})
    }

    handleChange = (e) => {
        //console.log(e.target.value)
        this.filterItems(e.target.value)
    }

    _mapCategoryName(categoryName) {
        for (let key in CATEGORIES) {
            if (key === categoryName) {
                return CATEGORIES[key].fullName
            }
        }
    }

    _mapFullCategoryName(categoryName) {
       return CATEGORIES[categoryName]['fullName']
    }

    categoryRouteValid = (route) => {
        if (Object.keys(CATEGORIES).indexOf(route) !== -1) {
            return true;
        } else {
            return false;
        }
    }


    render() {
        const {filteredData, isLoading} = this.state;

        let categories = []
        this.state.data.forEach((cur, idx) => {
            if (categories.indexOf(cur.kategorie) === -1) {
                categories.push(cur.kategorie)
            }
        })
        
        if (isLoading) {
            return <Loader></Loader>
        }
        
        // vyrendrujeme kategorii
        if (this.componentType === 'category') {
            // neplatny nazev kategorie
            if (!this.categoryRouteValid(this.state.categoryId)) {
                return <Redirect to={`${ROUTE_PREFIX}error`}/>
            }

            return (
                <div>
                    <nav className="subHeader__nav--small">
                        <div className="layout__inner">
                            <Link to={`${ROUTE_PREFIX}`}>Všechny čaje</Link>
                        </div>
                    </nav>
                    <div className="subHeader subHeader__category layout__inner">
                        <h1>
                            {this._mapFullCategoryName(this.state.categoryId)} čaje 
                            <span className={this.state.categoryId}></span>
                        </h1>

                        <section className="productList">
                            {filteredData.map(tea => {
                                tea['isCategory'] = true
                                //console.log(tea)
                                return <Article key={tea.druh_id} payload={tea}></Article> 
                            })}
                        </section>
                        
                    </div>
                </div>
            )
        } 
        // vyrenderujeme vsechny caje
        else {
            return (
                <div>
                    <section className="subHeader">
                        <div className="subHeader__hero">
                            <img src={hero} alt="Čaje všech druhů a chutí"/>
                        </div>
                        <div className="layout__inner">
                            <nav className="subHeader__nav--categories">
                                 {categories.map(category => <Link to={`/caje/${category}`} key={category} className={category}><span>{this._mapCategoryName(category)}</span></Link>)}
                            </nav>
                        </div>
                    </section>

                    <section className="productList">
                      {filteredData.map(tea => <Article key={tea.druh_id} payload={tea} type={this.componentType}></Article> )}
                    </section>
                </div>
            )
        }        
    }
}